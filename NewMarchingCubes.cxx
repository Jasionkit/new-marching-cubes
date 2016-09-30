#include <vtkVersion.h>
#include <vtkSmartPointer.h>
#include <vtkMarchingCubes.h>
#include <vtkVoxelModeller.h>
#include <vtkSphereSource.h>
#include <vtkImageData.h>
#include "vtkPNGReader.h"
#include <vtkSmoothPolyDataFilter.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkStripper.h>
#include "vtkMarchingCubesSpace.h"



int main(int argc, char *argv[])
{
	vtkSmartPointer<vtkImageData> OriginalVolume =vtkSmartPointer<vtkImageData>::New();
	vtkSmartPointer<vtkImageData> BinaryVolume =vtkSmartPointer<vtkImageData>::New();

	double isoValue=127;


	// �½��ļ���ȡ���󣬳�������vtkBMPReader��vtkDICOMImageReader��vtkJPEGReader��
	vtkSmartPointer<vtkPNGReader> BinaryPNGReader =vtkSmartPointer<vtkPNGReader>::New();  
	// ��ͬ��reader��Ҫ���õĲ����ǲ�ͬ�� ��˱������ʺ�jpegreader
	BinaryPNGReader->SetFilePrefix("G:/GraduationDesignNeeded/ExperimentalData/Interpolation/BinaryImage16/P01/"); // Ҫ�򿪵�·��
	BinaryPNGReader->SetFilePattern("%s%03d.png");// ͼƬ�ļ�����ʽ���˴�Ϊ 001.png 015.png ...
	BinaryPNGReader->SetDataByteOrderToLittleEndian();
	BinaryPNGReader->SetDataSpacing(1, 1, 1);  // ������ά���ݳ������ؼ�࣬
	BinaryPNGReader->SetFileNameSliceSpacing(1); 
	BinaryPNGReader->SetDataExtent(0, 109, 0, 109, 1, 30);   //������һ�㣬��ʹ��������һ�����������������һ����ͨ����
	// ������Ϊ��P01�ļ���������0.png ~ 15.png����������Ϊ 1��29
	// ÿ��ͼƬ�ĳ���Ϊ110 * 110 �������Ϊ0��109
	BinaryPNGReader->Update();  
	BinaryVolume->DeepCopy(BinaryPNGReader->GetOutput());


	// �½��ļ���ȡ���󣬳�������vtkBMPReader��vtkDICOMImageReader��vtkJPEGReader��
	vtkSmartPointer<vtkPNGReader> OriginalPNGReader =vtkSmartPointer<vtkPNGReader>::New();  
	// ��ͬ��reader��Ҫ���õĲ����ǲ�ͬ�� ��˱������ʺ�jpegreader
	OriginalPNGReader->SetFilePrefix("G:/GraduationDesignNeeded/ExperimentalData/Interpolation/OriginalImage/P01/"); // Ҫ�򿪵�·��
	OriginalPNGReader->SetFilePattern("%s%03d.png");// ͼƬ�ļ�����ʽ���˴�Ϊ 001.png 015.png ...
	OriginalPNGReader->SetDataByteOrderToLittleEndian();
	OriginalPNGReader->SetDataSpacing(1, 1, 1);  // ������ά���ݳ������ؼ�࣬
	OriginalPNGReader->SetFileNameSliceSpacing(1); 
	OriginalPNGReader->SetDataExtent(0, 109, 0, 109, 1, 30); //������һ�㣬��ʹ��������һ�����������������һ����ͨ����
	// ������Ϊ��P01�ļ���������0.png ~ 57.png����������Ϊ 1��29
	// ÿ��ͼƬ�ĳ���Ϊ110 * 110 �������Ϊ0��109
	OriginalPNGReader->Update(); 

	OriginalVolume->DeepCopy(OriginalPNGReader->GetOutput());

	


	vtkSmartPointer<vtkMarchingCubesSpace> surface = vtkSmartPointer<vtkMarchingCubesSpace>::New();

#if VTK_MAJOR_VERSION <= 5
  surface->SetInput(BinaryVolume);   //5��ǰ�汾��֧��
#else
  surface->SetInputData(0,BinaryVolume);    //0�Ŷ˿���Ϊ��ֵ��ͼ������
  surface->SetInputData(1,OriginalVolume);  //1�Ŷ˿���Ϊԭʼͼ������˿�,���0�ź�1�Ŷ˿ڶ�����ͬһ�����ݣ����������㲻��
#endif
  surface->ComputeNormalsOn();
  //surface->ComputeNormalsOff();  //�����off����������Ĭ��Ϊ�����ֵ��ͼ��ķ��������㣬Ĭ��Ϊon
  surface->SetValue(0, isoValue);


  vtkSmartPointer<vtkStripper> stripper = 
      vtkSmartPointer<vtkStripper>::New();
  stripper->SetInputConnection(surface->GetOutputPort());  //�����ɵ�����Ƭ���ӳ����Ǵ� 
  stripper->Update();


  vtkSmartPointer<vtkSmoothPolyDataFilter> smoothFilter =
	  vtkSmartPointer<vtkSmoothPolyDataFilter>::New();
  smoothFilter->SetInputConnection(stripper->GetOutputPort());
  smoothFilter->SetNumberOfIterations(5);
  smoothFilter->Update();


  vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  mapper->SetInputConnection(smoothFilter->GetOutputPort());
  mapper->ScalarVisibilityOff();

  vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
  actor->SetMapper(mapper);   actor->GetProperty()->SetColor(1,0,0);


  vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
   renderer->SetBackground(1, 1, 1);  

  vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
  renderWindow->AddRenderer(renderer);
  vtkSmartPointer<vtkRenderWindowInteractor> interactor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
  interactor->SetRenderWindow(renderWindow);

  renderer->AddActor(actor);

  renderWindow->Render();
  interactor->Start();

  return EXIT_SUCCESS;
}
