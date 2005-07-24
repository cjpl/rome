// Author: R. Sawada
//////////////////////////////////////////////////////////////////////////
//
//  ROMETextDataBase
//
//  Text format non-relational database.
//
//  $Log$
//  Revision 1.2  2005/07/24 17:37:54  sawada
//  added row number comment.
//
//  Revision 1.1  2005/07/24 16:52:47  sawada
//  added ROMETextDataBase.
//
//
//////////////////////////////////////////////////////////////////////////
#include <ROMETextDataBase.h>
#include <TTimeStamp.h>

const char* EndOfHeader = "/////////////////////////////////////----///////////////////////////////////////";

ROMETextDataBase::ROMETextDataBase() {
}

ROMETextDataBase::~ROMETextDataBase() {
}

bool ROMETextDataBase::Init(const char* name,const char* path,const char* connection) {
   // set directory
   // "connection" has no mean for this class.
   if(strlen(path)){
      fDirectoryPath = path;
   }
   else{
      fDirectoryPath = ".";
   }
   fDirectoryPath += "/";

   fName = name;

   return true;
}

bool ROMETextDataBase::Read(ROMEStr2DArray *values,const char *dataBasePath,int runNumber) {
   fstream*   fileStream;
   int        ps,pe;
   int        iRow,iCol;
   ROMEString lineBuffer = "";
   ROMEString fileName;
   ROMEString valueName;
   ROMEString tmp;

   // initialize
   RemoveComment(lineBuffer,true);

   // extract value name and file name
   fileName = dataBasePath;
   // remove + and - at the end of file name
   if(fileName(fileName.Length()-1) == '+'){
      fileName.Resize(fileName.Length()-1);
   }
   if(fileName(fileName.Length()-1) == '-'){
      fileName.Resize(fileName.Length()-1);
   }
   ps = pe = 0;
   while((ps=fileName.Index("/",1,pe,TString::kExact))!=-1){
      pe = ps +1;
   }
   if(!pe){
      valueName = "Value";
   }
   else{
      valueName = fileName(pe,fileName.Length()-pe);
      fileName.Remove(pe-1);
   }
   fileName.Prepend(fDirectoryPath);

   // open file
   if(!(fileStream = new fstream(fileName.Data(),ios::in)))
      return true;

   // read data
   bool isData = false;
   iRow = 0;
   while(lineBuffer.ReadLine(*fileStream)){
      RemoveComment(lineBuffer);
      if(!ContainsData(lineBuffer.Data()))
         continue;

      if(lineBuffer.Contains(StartMark(valueName.Data())))
         isData = true;

      if(isData && lineBuffer.Contains(EndMark(valueName.Data()))){
         isData = false;
         break;
      }

      if(isData){
         iCol=0;
         ps = pe = 0;
         // skip start mark
         lineBuffer.ReplaceAll(StartMark(valueName.Data()),"");
         if(!ContainsData(lineBuffer.Data()))
            continue;

         while((ps=lineBuffer.Index(",",1,pe,TString::kExact))!=-1){
            tmp = lineBuffer(pe,ps-pe);
            values->SetAt(tmp,iRow,iCol);
            pe = ps+1;
            iCol++;
         }
         tmp = lineBuffer(pe,lineBuffer.Length()-pe);
         values->SetAt(tmp,iRow,iCol);
         iRow++;
      }
   }

   delete fileStream;
   return true;
}

bool ROMETextDataBase::Write(ROMEStr2DArray* values,const char *dataBasePath,int runNumber) {
   fstream*   fileStream;
   ROMEString fileBuffer;
   ROMEString buffer;
   ROMEString fileName;
   ROMEString valueName;
   int        iRow,iCol;
   bool       append  = false;
   bool       prepend = false;
   int        ps,pe;

   // extract value name and file name
   fileName = dataBasePath;
   if(fileName(fileName.Length()-1) == '+'){
      append = true;
      fileName.Resize(fileName.Length()-1);
   }
   if(fileName(fileName.Length()-1) == '-'){
      prepend = true;
      fileName.Resize(fileName.Length()-1);
   }
   ps = pe = 0;
   while((ps=fileName.Index("/",1,pe,TString::kExact))!=-1){
      pe = ps +1;
   }
   if(!pe){
      valueName = "Value";
   }
   else{
      valueName = fileName(pe,fileName.Length()-pe);
      fileName.Remove(pe-1);
   }
   fileName.Prepend(fDirectoryPath);

   // read existing file
   if(append || prepend){
      if(!(fileStream = new fstream(fileName.Data(),ios::in))){
         append = prepend = false;
      }
      else{
         fileBuffer.ReadFile(*fileStream);
         delete fileStream;
      }
   }

   // format data
   buffer.Resize(0);
   buffer.AppendFormatted("%s\n",StartMark(valueName.Data()));
   for(iRow=0;iRow<values->GetEntries();iRow++){
      for(iCol=0;iCol<values->GetEntriesAt(iRow);iCol++){
         buffer += values->At(iRow,iCol);
         if(iCol == values->GetEntriesAt(iRow)-1){
            if(values->GetEntries()>1)
               buffer.AppendFormatted("\t//%5d",iRow);
            buffer += "\n";
         }
         else{
            buffer += ",\t";
         }
      }
   }
   buffer.AppendFormatted("%s\n",EndMark(valueName.Data()));
   buffer.AppendFormatted("\n");

   RemoveHeader(fileBuffer);

   if(append)
      buffer.Prepend(fileBuffer);
   else if(prepend)
      buffer.Append(fileBuffer);

   AddHeader(buffer,fileName.Data());

   // write file
   if(!(fileStream = new fstream(fileName.Data(),ios::out | ios::trunc))){
      cout << "\n\nError : Failed to open '" << fileName << "' !!!" << endl;
      return false;
   }
   *fileStream<<buffer;

   delete fileStream;
   return true;
}

void ROMETextDataBase::RemoveComment(ROMEString &buffer,bool initialize) {
   static bool inComment;
   int ps,pe;

   if(initialize)
      inComment = false;

// remove after "//"
   if((ps = buffer.Index("//",2,0,TString::kExact)) != -1){
      buffer.Remove(ps);
   }

// remove between "/*" and "*/"
   if(inComment){
      if((pe = buffer.Index("*/",2,0,TString::kExact)) != -1){
         buffer.Remove(0,pe+strlen("*/"));
         inComment = false;
      }
      else{
         buffer.Resize(0);
      }
   }
   ps = pe = 0;
   while((ps = buffer.Index("/*",2,pe,TString::kExact)) != -1){
      inComment = true;
      if((pe = buffer.Index("*/",2,ps,TString::kExact)) != -1){
         buffer.Remove(ps,pe-ps+strlen("*/"));
         inComment = false;
         pe = ps;
      }
      else{
         pe = ps + strlen("/*");
      }
   }
   if(inComment)
      buffer.Remove(pe-strlen("/*"));

   return;
}

bool ROMETextDataBase::ContainsData(const char* buffer) {
   // return true if "buffer" contains non-space characters
   if(!buffer)
      return false;

   ROMEString tmp = buffer;
   tmp.ReplaceAll(" ","");
   tmp.ReplaceAll("\t","");

   if(tmp.Length())
      return true;

   return false;
}

const char* ROMETextDataBase::StartMark(const char* valueName) {
   static ROMEString mark;
   mark.SetFormatted("<%s>",valueName);
   return mark.Data();
}

const char* ROMETextDataBase::EndMark(const char* valueName) {
   static ROMEString mark;
   mark.SetFormatted("</%s>",valueName);
   return mark.Data();
}

void ROMETextDataBase::AddHeader(ROMEString &buffer,const char* fileName) {
   TTimeStamp time;
   time.Set();
   ROMEString filename = fileName;
   ROMEString tmp;
   ROMEStrArray valueNames;

   int pBuffer=-1;
   int ps,pe;

   ps = pe = 0;
   while((ps=filename.Index("/",1,pe,TString::kExact))!=-1){
      pe = ps+1;
   }
   if(pe)
      filename.Remove(0,pe);

   ROMEString header;
   header.Resize(0);
   header.AppendFormatted("////////////////////////////////////////////////////////////////////////////////\n");
   header.AppendFormatted("//\n");
   header.AppendFormatted("// %s\n",filename.Data());
   header.AppendFormatted("// %s\n",time.AsString("l"));
   header.AppendFormatted("//\n");
   header.AppendFormatted("// This file contains following data.\n");
   ps = pe = 0;
   int i;
   while((ps=buffer.Index("<",1,pe,TString::kExact))!=-1){
      pe=buffer.Index(">",1,ps,TString::kExact);
      if((ps=buffer.Index("</",2,pe,TString::kExact))==-1)
         break;
      pe=buffer.Index(">",1,ps,TString::kExact);
      tmp = buffer(ps+2,pe-ps-2);
      for(i=0;i<valueNames.GetEntries();i++){
         if(tmp == valueNames.At(i))
            break;
      }
      if(i==valueNames.GetEntries()){
         valueNames.AddAt(tmp,i);
         header.AppendFormatted("//    %s\n",tmp.Data());
      }
      pe = ps + 1;
   }
   header.AppendFormatted("//\n");
   header.AppendFormatted("%s\n",EndOfHeader);

   buffer.Prepend(header);

   return;
}

void ROMETextDataBase::RemoveHeader(ROMEString &buffer) {
   int pBuffer = buffer.Index(EndOfHeader,strlen(EndOfHeader),0,TString::kExact);
   if(pBuffer>=0)
      buffer.Remove(0,pBuffer+strlen(EndOfHeader)+1);
   return;
}
