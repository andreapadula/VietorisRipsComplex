#include                  <ttkVietorisRipsComplex.h>
vtkStandardNewMacro(ttkVietorisRipsComplex)

int ttkVietorisRipsComplex::doIt(vector<vtkDataSet *> &inputs, vector<vtkDataSet *> &outputs){

  Memory m;
  
  vtkPointSet *input =vtkPointSet::SafeDownCast (inputs[0]);
  vtkUnstructuredGrid *output = vtkUnstructuredGrid::SafeDownCast(outputs[0]);
    
  Triangulation *triangulation = ttkTriangulation::getTriangulation(input);
 
  if(!triangulation)
    return -1;
  
  triangulation->setWrapper(this);
  vietorisRipsComplex_.setupTriangulation(triangulation);
  vietorisRipsComplex_.setWrapper(this);

    
  // in the following, the target scalar field of the input is replaced in the 
  // variable 'output' with the result of the computation.
  // if your wrapper produces an output of the same type of the input, you 
  // should proceed in the same way.
    vtkPoints *inputPointSet = (input)->GetPoints();
    int size1=input->GetNumberOfPoints();
    bool matrix[size1][size1];
    memset(matrix,false,sizeof(matrix));
    //vtkPoints *outputPointSet = (vtkPointSet::SafeDownCast(output))->GetPoints();
    vector<vector<int>> triangles;
    vector<vector<int>> tetras;
    vector<vector<int>> edges;

    
//    Radius=0.3;
    cout<<"Option1:"<<Option1<<endl;
    cout<<"Option2:"<<Option2<<endl;
    cout<<"Option3:"<<Option3<<endl;
    
    
  // calling the executing package
    for (int i=0; i<input->GetNumberOfPoints()-1; i++) {
        for (int j=i+1; j<input->GetNumberOfPoints(); j++) {
            double * mypointer;
            mypointer= input->GetPoint(i);
            double a[3];
            a[0]=mypointer[0];
            a[1]=mypointer[1];
            a[2]=mypointer[2];
            double * mypointer2;
            mypointer2= input->GetPoint(j);
            double a2[3];
            a2[0]=mypointer2[0];
            a2[1]=mypointer2[1];
            a2[2]=mypointer2[2];
            double distance = 0;
            
            for(int i = 0; i < 3; i++){
                distance += (a[i] - a2[i])*(a[i] - a2[i]);
            }
            distance=sqrt(distance);
                if (distance <= 2*Radius) {
                    vector<int> edge;
                    edge.push_back(i);
                    edge.push_back(j);
                    edges.push_back(edge);
                    matrix[i][j]=true;
                    matrix[j][i]=true;
                }
        }
    }
    
    
    vtkUnstructuredGrid * grid=vtkUnstructuredGrid::New();
    grid->Allocate();
    grid->SetPoints(inputPointSet);
    if(Option1){
        cout<<"EDGES "<<edges.size()<<endl;
        vtkIdType ids[2];
        for (int i=0; i<edges.size(); i++) {
            ids[0]=(edges.at(i)[0]);
            ids[1]=(edges.at(i)[1]);
            grid->InsertNextCell(VTK_LINE,2,ids);
            
        }
    }

    
    if(Option2||Option3){
        
        int found=0;
        for (int i = 0; i < input->GetNumberOfPoints(); i++) {
            for (int j = i+1; j < input->GetNumberOfPoints(); j++) {
                found=0;
                if(matrix[i][j]==true){
                    found++;
                }
                if (found==1){
                    for (int k = j+1; k < input->GetNumberOfPoints(); k++) {
                        found=1;
                            if(matrix[i][k]==true){
                                found++;
                                if(matrix[j][k]==true){
                                    found++;
                                }
                            }
                            if (found==3) {
                                if(Option2){
                                    vector<int> triangle;
                                    triangle.push_back(i);
                                    triangle.push_back(j);
                                    triangle.push_back(k);
                                    triangles.push_back(triangle);
                                }
                                if (Option3) {
                                    for (int r=k+1; r < input->GetNumberOfPoints(); r++) {
                                        found=3;
                                        if(matrix[i][r]==true){
                                            found++;
                                            if(matrix[j][r]==true){
                                                found++;
                                                if(matrix[k][r]==true){
                                                    found++;
                                                }
                                            }
                                        }
                                        if (found==6) {
                                            found=3;
                                            vector<int> tetra;
                                            tetra.push_back(i);
                                            tetra.push_back(j);
                                            tetra.push_back(k);
                                            tetra.push_back(r);
                                            tetras.push_back(tetra);
                                        }
                                    }
                                }
                        
                            }
                        
                    }
                }
            }
        }
    }
    
    
    if(Option2){
        cout<<"TRIANGLES "<<triangles.size()<<endl;
        vtkIdType Triangleids[3];
        for (int i=0; i<triangles.size(); i++) {
            Triangleids[0]=(triangles.at(i)[0]);
            Triangleids[1]=(triangles.at(i)[1]);
            Triangleids[2]=(triangles.at(i)[2]);
            grid->InsertNextCell(VTK_TRIANGLE,3,Triangleids);
            
        }
    }
    
    if(Option3){
        cout<<"TETRAS "<<tetras.size()<<endl;
        vtkIdType TetraIds[4];
        for (int i=0; i<tetras.size(); i++) {
            TetraIds[0]=(tetras.at(i)[0]);
            TetraIds[1]=(tetras.at(i)[1]);
            TetraIds[2]=(tetras.at(i)[2]);
            TetraIds[3]=(tetras.at(i)[3]);
            grid->InsertNextCell(VTK_TETRA,4,TetraIds);
            
        }
    }    
    output->ShallowCopy(grid);
    
//  switch(outputPointSet->GetDataType()){
   
//    vtkTemplateMacro((
//      {
//        vietorisRipsComplex_.setInputDataPointer(inputPointSet->GetVoidPointer(0));
//        vietorisRipsComplex_.setOutputDataPointer(outputPointSet->GetVoidPointer(0));
//        vietorisRipsComplex_.execute<VTK_TT>(Radius,Option1,Option2,Option3);
//      }
//    ));
//  }
  
  {
    stringstream msg;
    msg << "[ttkVietorisRipsComplex] Memory usage: " << m.getElapsedUsage() 
      << " MB." << endl;
    dMsg(cout, msg.str(), memoryMsg);
  }
  
  return 0;
}

int ttkVietorisRipsComplex::FillOutputPortInformation(int port, vtkInformation* info){
    if(port == 0)
    info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkUnstructuredGrid");
    
    return 1;
}
int ttkVietorisRipsComplex::FillInputPortInformation(int port, vtkInformation* info){
    if(port == 0)
        info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkPointSet");
    
    return 1;
}
