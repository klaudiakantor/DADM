#include <vtkSmartPointer.h>
#include <vtkObjectFactory.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkActor.h>
#include <vtkImageViewer2.h>
#include <vtkDICOMImageReader.h>
#include <vtkInteractorStyleImage.h>
#include <vtkActor2D.h>
#include <vtkTextProperty.h>
#include <vtkTextMapper.h>
#include <sstream>

class StatusMessage {
public:
   static std::string Format(int slice, int maxSlice) {
      std::stringstream tmp;
      tmp << "Slice " << slice + 1 << "/" << maxSlice + 1;
      return tmp.str();
   }
};


class myVtkInteractorStyleImage : public vtkInteractorStyleImage
{
public:
   static myVtkInteractorStyleImage* New();
   vtkTypeMacro(myVtkInteractorStyleImage, vtkInteractorStyleImage);

protected:
   vtkImageViewer2* _ImageViewer;
   vtkTextMapper* _StatusMapper;
   int _Slice;
   int _MinSlice;
   int _MaxSlice;

public:
   void SetImageViewer(vtkImageViewer2* imageViewer) {
      _ImageViewer = imageViewer;
      _MinSlice = imageViewer->GetSliceMin();
      _MaxSlice = imageViewer->GetSliceMax();
      _Slice = _MinSlice;
   }

   void SetStatusMapper(vtkTextMapper* statusMapper) {
      _StatusMapper = statusMapper;
   }

protected:
   void MoveSliceForward() {
      if(_Slice < _MaxSlice) {
         _Slice += 1;
         _ImageViewer->SetSlice(_Slice);
         std::string msg = StatusMessage::Format(_Slice, _MaxSlice);
         _StatusMapper->SetInput(msg.c_str());
         _ImageViewer->Render();
      }
   }

   void MoveSliceBackward() {
      if(_Slice > _MinSlice) {
         _Slice -= 1;
         _ImageViewer->SetSlice(_Slice);
         std::string msg = StatusMessage::Format(_Slice, _MaxSlice);
         _StatusMapper->SetInput(msg.c_str());
         _ImageViewer->Render();
      }
   }

   virtual void OnKeyDown() {
      std::string key = this->GetInteractor()->GetKeySym();
      if(key.compare("Up") == 0) {
         MoveSliceForward();
      }
      else if(key.compare("Down") == 0) {
         MoveSliceBackward();
      }
      vtkInteractorStyleImage::OnKeyDown();
   }

   virtual void OnMouseWheelForward() {
      MoveSliceForward();
   }

   virtual void OnMouseWheelBackward() {
      if(_Slice > _MinSlice) {
         MoveSliceBackward();
      }
   }
};

vtkStandardNewMacro(myVtkInteractorStyleImage);


int main(int argc, char* argv[])
{
   std::string folder = "\dicom_images";

   vtkSmartPointer<vtkDICOMImageReader> reader =
      vtkSmartPointer<vtkDICOMImageReader>::New();
   reader->SetDirectoryName(folder.c_str());
   reader->Update();

   vtkSmartPointer<vtkImageViewer2> imageViewer =
      vtkSmartPointer<vtkImageViewer2>::New();
   imageViewer->SetInputConnection(reader->GetOutputPort());

   vtkSmartPointer<vtkTextProperty> sliceTextProp = vtkSmartPointer<vtkTextProperty>::New();
   sliceTextProp->SetFontFamilyToCourier();
   sliceTextProp->SetFontSize(20);
   sliceTextProp->SetVerticalJustificationToBottom();
   sliceTextProp->SetJustificationToLeft();

   vtkSmartPointer<vtkTextMapper> sliceTextMapper = vtkSmartPointer<vtkTextMapper>::New();
   std::string msg = StatusMessage::Format(imageViewer->GetSliceMin(), imageViewer->GetSliceMax());
   sliceTextMapper->SetInput(msg.c_str());
   sliceTextMapper->SetTextProperty(sliceTextProp);

   vtkSmartPointer<vtkActor2D> sliceTextActor = vtkSmartPointer<vtkActor2D>::New();
   sliceTextActor->SetMapper(sliceTextMapper);
   sliceTextActor->SetPosition(15, 10);
 
   vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
      vtkSmartPointer<vtkRenderWindowInteractor>::New();

   vtkSmartPointer<myVtkInteractorStyleImage> myInteractorStyle =
      vtkSmartPointer<myVtkInteractorStyleImage>::New();

   myInteractorStyle->SetImageViewer(imageViewer);
   myInteractorStyle->SetStatusMapper(sliceTextMapper);

   imageViewer->SetupInteractor(renderWindowInteractor);

   renderWindowInteractor->SetInteractorStyle(myInteractorStyle);

   imageViewer->GetRenderer()->AddActor2D(sliceTextActor);

   imageViewer->GetRenderWindow()->SetSize(700, 600);
   imageViewer->Render();
   imageViewer->GetRenderWindow()->SetWindowName("Visualization 2D");
   imageViewer->GetRenderer()->ResetCamera();
   imageViewer->Render();
   renderWindowInteractor->Start();
   return EXIT_SUCCESS;
}
