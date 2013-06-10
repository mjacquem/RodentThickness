#include "iostream"
#include "fstream"
#include "vtkPolyData.h"
#include "vtkPolyDataReader.h"
#include "vtkPolyDataWriter.h"
#include "itkAffineTransform.h"
#include "itkBSplineDeformableTransform.h"
#include "itkTransformFileReader.h"
#include "itkTransformMeshFilter.h"
#include "itkTransform.h"
#include "itkMesh.h"
#include "itkDefaultStaticMeshTraits.h"
#include "itkPoint.h"

#include <itkImage.h>
#include <itkImageFileReader.h> 
#include <itkImageFileWriter.h>
#include "argio.h"

using namespace std;

typedef float FloatType;
typedef short ShortPixelType;
typedef unsigned short PixelType;
typedef unsigned char BinaryPixelType;

enum { ImageDimension = 3 };

typedef itk::Image<PixelType, ImageDimension>       ImageType;
typedef itk::Image<BinaryPixelType, ImageDimension> BinaryImageType;
typedef itk::Image<ShortPixelType, ImageDimension>  ShortImageType;
typedef itk::Image<FloatType, ImageDimension>       FloatImageType;
typedef itk::ImageFileReader< ImageType >          VolumeReaderType;
typedef itk::ImageFileWriter< ImageType >          VolumeWriterType;
typedef itk::ImageFileReader< BinaryImageType >    BinaryVolumeReaderType;
typedef itk::ImageFileWriter< BinaryImageType >    BinaryVolumeWriterType;
typedef itk::ImageFileReader< ShortImageType >     ShortVolumeReaderType;
typedef itk::ImageFileWriter< ShortImageType >     ShortVolumeWriterType;
typedef itk::Transform<float, 3, 3> TransformType;
typedef itk::AffineTransform<double, 3> AffineTransformType;
typedef itk::BSplineDeformableTransform<double, 3, 3> BsplineTransformType;
typedef itk::DefaultStaticMeshTraits<double, 3, 3, float, float, float> MeshTraits;
typedef itk::Mesh<double, 3, MeshTraits> MeshType;
typedef itk::TransformMeshFilter<MeshType, MeshType, TransformType> MeshFilterType;
typedef itk::Point<double, 3> PointType;
typedef itk::ImageRegionIterator< ImageType >      Iterator;

typedef ImageType::RegionType                 ImageRegionType;
typedef ImageType::Pointer                    ImagePointer;
typedef ShortImageType::Pointer               ShortImagePointer;

double __imageSpacing[3];
double __origin[3];

void usage() {
	cout << "Usage: MeshBsplineTransform -in inputmesh " << endl;
	cout << "			-out outputmesh " << endl;
	cout << "			-affine affine transformfile " << endl;
	cout << "			-bspline bspline transformfile " << endl;
	cout << "			-lst 'point list file for output mesh' " << endl;
	cout << "			-scale x,y,z" << endl << endl;
	cout << "			-flipXY" << endl;
}

void flipXY(vtkPoints* points) {
	for (int i = 0; i < points->GetNumberOfPoints(); i++) {
		double vtkPts[3];
		points->GetPoint(i, vtkPts);
		PointType pts;
		for (int j = 0; j < 3; j++) {
			pts[j] = vtkPts[j];
		}
		points->SetPoint(i, -pts[0], -pts[1], pts[2]);
	}
}

void applyBsplineTransform(vtkPoints* points, BsplineTransformType::Pointer transformer) {
	int n = 0;
	cout << "Applying bspline transformation" << endl;

	BsplineTransformType::Pointer invTransform = BsplineTransformType::New();
	for (int i = 0; i < points->GetNumberOfPoints(); i++, n++) {
		double vtkPts[3];
		points->GetPoint(i, vtkPts);
		PointType srcpts, dstpts;
		for (int j = 0; j < 3; j++) {
			srcpts[j] = vtkPts[j];// + 12.032;
		}
		dstpts = transformer->TransformPoint(srcpts);
		for (int j = 0; j < 3; j++) {
			dstpts[j] = dstpts[j];// - 12.032;
		}
		points->SetPoint(i, dstpts[0], dstpts[1], dstpts[2]);
		//cout << "(" << srcpts[0] << "," << srcpts[1] << "," << srcpts[2] << ") => (" << dstpts[0] << "," << dstpts[1] << "," << dstpts[2] << ")" << endl;
	}
	cout << "Transformed points: " << n << endl;
}

void applyAffineTransform(vtkPoints* points, AffineTransformType::Pointer transformer) {
	for (int i = 0; i < points->GetNumberOfPoints(); i++) {
		double vtkPts[3];
		points->GetPoint(i, vtkPts);
		PointType pts;
		for (int j = 0; j < 3; j++) {
			pts[j] = vtkPts[j];
		}
		pts = transformer->TransformPoint(pts);
		points->SetPoint(i, pts[0], pts[1], pts[2]);
	}
}

void writePointListFile(vtkPoints* points, const char* pointListOut) {
	ofstream out(pointListOut);	

	cout << "writing " << points->GetNumberOfPoints() << " points " << endl;
	for (int i = 0; i < points->GetNumberOfPoints(); i++) {
		double vtkPts[3];
		points->GetPoint(i, vtkPts);
		PointType pts;
		for (int j = 0; j < 3; j++) {
			pts[j] = vtkPts[j];
		}
		out << pts[0] << " " << pts[1] << " " << pts[2] << endl;
	}
	out.close();
}

void scale(vtkPoints* points, float scale[3]) {
	for (int i = 0; i < points->GetNumberOfPoints(); i++) {
		double vtkPts[3];
		points->GetPoint(i, vtkPts);
		PointType pts;
		for (int j = 0; j < 3; j++) {
			pts[j] = vtkPts[j];
			pts[j] = scale[j] * pts[j];
		}
		points->SetPoint(i, pts[0], pts[1], pts[2]);
	}
}

int main(const int argc, const char** argv) {
	if (argc < 3) {
		usage();
		return 0;
	}

	const char* input = ipGetStringArgument(argv, "-in", NULL);
	const char* output = ipGetStringArgument(argv, "-out", NULL);
	const char* affine = ipGetStringArgument(argv, "-affine", NULL);
	const char* bspline = ipGetStringArgument(argv, "-bspline", NULL);
	bool scaleOn = ipExistsArgument(argv, "-scale");
	const char* lstfile = ipGetStringArgument(argv, "-lst", NULL);
	const char* imageName = ipGetStringArgument(argv, "-image", NULL);
	bool flipXYOn = ipExistsArgument(argv, "-flipXY");

	if (NULL == input) {
		cout << "no input " << endl;
		usage();
		return 0;
	}
	if (NULL == affine && NULL == bspline && !scaleOn && !flipXYOn) {
		cout << "no operation " << endl;
		usage();
		return 0;
	}
	if (NULL == output && NULL == lstfile) {
		cout << "no output " << endl;
		usage();
		return 0;
	}


	vtkPolyData* inputMesh  = NULL;
	vtkPolyData* outputMesh = NULL;

	vtkPolyDataReader* reader = vtkPolyDataReader::New();
	reader->SetFileName(input);
	reader->Update();
	inputMesh = reader->GetOutput();	

	outputMesh = vtkPolyData::New();
	outputMesh->DeepCopy(inputMesh);

	vtkPoints* points = outputMesh->GetPoints();
	if (flipXYOn) {
		flipXY(points);
	}	

	if (affine != NULL) {
		typedef itk::TransformFileReader TransformReaderType;
		TransformReaderType::Pointer transformReader = TransformReaderType::New();

		cout << "loading affine transform file: " << affine << endl;
		transformReader->SetFileName(affine);
		transformReader->Update();

		while (transformReader->GetTransformList()->size()) {
			AffineTransformType::Pointer affineTfm = dynamic_cast<AffineTransformType*>(transformReader->GetTransformList()->front().GetPointer());				
			if (affineTfm) {
				applyAffineTransform(points, affineTfm);
			} else {
				cout << "can't recognize affine transformation " << affine << endl;
			}
			transformReader->GetTransformList()->pop_front();
		}
	}

	if (bspline != NULL) {
		typedef itk::TransformFileReader TransformReaderType;
		TransformReaderType::Pointer transformReader = TransformReaderType::New();

		cout << "loading bspline transform file: " << bspline << endl;
		transformReader->SetFileName(bspline);
		transformReader->Update();

		while (transformReader->GetTransformList()->size()) {
			BsplineTransformType::Pointer bsplineTfm = dynamic_cast<BsplineTransformType*>(transformReader->GetTransformList()->front().GetPointer());
			AffineTransformType::Pointer affineTfm = dynamic_cast<AffineTransformType*>(transformReader->GetTransformList()->front().GetPointer());				
			if (bsplineTfm) {
				cout << bsplineTfm << endl;
				applyBsplineTransform(points, bsplineTfm);
			} else if (affineTfm) {
				applyAffineTransform(points, affineTfm);
			} else {
				cout << "can't recognize non-rigid transformation " << bspline << endl;
			}
			transformReader->GetTransformList()->pop_front();
		}
	}

	if (scaleOn) {
		const char* tmp_str = ipGetStringArgument(argv, "-scale", NULL);
		float pixdims[3];
		if (tmp_str) { 
			int num = ipExtractFloatTokens(pixdims, tmp_str, 3);
			if (3 != num) {
				cerr << "editPixdims needs 3 comma separated entries: px,py,pz " << endl;
				exit(1); 
			} 
		}  
		scale(points, pixdims);
	}

	if (output != NULL) {
		vtkPolyDataWriter* writer = vtkPolyDataWriter::New();
		writer->SetFileName(output);
		writer->SetInput(outputMesh);
		writer->Write();
		writer->Delete();
	}

	if (lstfile != NULL) {
		cout << "writing " << lstfile << endl;
		writePointListFile(outputMesh->GetPoints(), lstfile);		
	}

	reader->Delete();
	return 0;
}
