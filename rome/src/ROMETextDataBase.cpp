// Author: R. Sawada
//////////////////////////////////////////////////////////////////////////
//
//  ROMETextDataBase
//
//  Text format non-relational database.
//
//  $Id$
//
//////////////////////////////////////////////////////////////////////////
#include <ROMETextDataBase.h>
#include <TArrayI.h>

const char* EndOfHeader = "/////////////////////////////////////----///////////////////////////////////////";
const int   numbering = 10;

ClassImp(ROMETextDataBase)

ROMETextDataBase::ROMETextDataBase() {
   fTime = new TTimeStamp();
}

ROMETextDataBase::~ROMETextDataBase() {
   delete fTime;
}

Bool_t ROMETextDataBase::Init(const char* name,const char* path,const char* connection) {
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

Bool_t ROMETextDataBase::Read(ROMEStr2DArray *values,const char *dataBasePath,Long64_t runNumber,Long64_t eventNumber) {
   Ssiz_t     ps,pe;
   int        iRow;
   int        iCol = 0;
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
   fstream    fileStream(fileName.Data(),ios::in);
   if(!fileStream)
      return true;

   // read data
   bool start = false;
   bool end = false;
   bool lineEndsWithComma = false;

   iRow = 0;
   while(lineBuffer.ReadLine(fileStream)){
      if(end)
         break;

      RemoveComment(lineBuffer);
      if(!ContainsData(lineBuffer.Data()))
         continue;

      if(lineBuffer.Contains(StartMark(valueName.Data())))
         start = true;

      if(start){
         lineBuffer.StripSpaces();
         if (lineEndsWithComma) {
            lineEndsWithComma = false;
         }
         else {
            iCol=0;
         }
         ps = pe = 0;
         // skip start mark
         lineBuffer.ReplaceAll(StartMark(valueName.Data()),"");
         if((ps=lineBuffer.Index(EndMark(valueName.Data()),strlen(EndMark(valueName.Data())),0,TString::kExact))!=-1){
            end = true;
            lineBuffer.ReplaceAll(EndMark(valueName.Data()),"");
         }

         if(!ContainsData(lineBuffer.Data()))
            continue;

        while((ps=lineBuffer.Index(",",1,pe,TString::kExact))!=-1){
           tmp = lineBuffer(pe,ps-pe);
           tmp.StripSpaces();
           values->SetAt(tmp,iRow,iCol);
           pe = ps+1;
           iCol++;
           if (ps == lineBuffer.Length()-1) {
              lineEndsWithComma = true;
              break;
           }
        }
        if (!lineEndsWithComma) {
           tmp = lineBuffer(pe,lineBuffer.Length()-pe);
           tmp.StripSpaces();
           values->SetAt(tmp,iRow,iCol);
           iRow++;
        }
      }
   }

   return true;
}

Bool_t ROMETextDataBase::Write(ROMEStr2DArray* values,const char *dataBasePath,Long64_t runNumber,Long64_t eventNumber) {
   fstream*   fileStream;
   ROMEString fileBuffer;
   ROMEString buffer;
   ROMEString fileName;
   ROMEString valueName;
   ROMEString format;
   int        iRow,iCol;
   bool       append  = false;
   bool       prepend = false;
   Ssiz_t     ps,pe;
   TArrayI    fieldLen;

#if (ROOT_VERSION_CODE >= ROOT_VERSION(4,0,0))
   fTime->Set();
#endif

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

   const int kColPerLine = 5;

   // count field length
   for(iRow=0;iRow<values->GetEntries();iRow++){
      if(values->GetEntriesAt(iRow)>fieldLen.GetSize())
         fieldLen.Set(values->GetEntriesAt(iRow));
      for(iCol=0;iCol<values->GetEntriesAt(iRow);iCol++){
         if(fieldLen.At(iCol%kColPerLine)<(Ssiz_t)strlen(values->At(iRow,iCol)))
            fieldLen.AddAt(strlen(values->At(iRow,iCol)),iCol%kColPerLine);
      }
   }

   // format data
   buffer.Resize(0);
   if(append || prepend)
      buffer.AppendFormatted("// %s\n",fTime->AsString("l"));
   buffer.AppendFormatted("%s",StartMark(valueName.Data()));
   if(values->GetEntries()>1)
      buffer += "\n";
   for(iRow=0;iRow<values->GetEntries();iRow++){
      for(iCol=0;iCol<fieldLen.GetSize();iCol++){
         format.SetFormatted(" %%%ds",fieldLen.At(iCol%kColPerLine));
         if(iCol<values->GetEntriesAt(iRow))
            buffer.AppendFormatted(format.Data(),values->At(iRow,iCol).Data());
         else
            buffer.AppendFormatted(format.Data(),"");
         if(iCol == fieldLen.GetSize()-1){
            if(values->GetEntries()>numbering)
               buffer.AppendFormatted("        //%5d",iRow);
            buffer += "\n";
         }
         else if ((iCol+1) % kColPerLine == 0) {
            buffer += ",\n";
         }
         else{
            buffer += ",";
         }
      }
   }
   if (values->GetEntries() == 1)
      buffer.Resize(buffer.Length()-1); // remove the last '\n'
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

void ROMETextDataBase::RemoveComment(ROMEString &buffer,Bool_t initialize) {
   static bool inComment;
   Ssiz_t ps,pe;

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

Bool_t ROMETextDataBase::ContainsData(const char* buffer) {
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
   ROMEString filename = fileName;
   ROMEString tmp;
   ROMEStrArray valueNames;

   Ssiz_t ps,pe;

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
   header.AppendFormatted("// %s\n",fTime->AsString("l"));
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
   Ssiz_t pBuffer = buffer.Index(EndOfHeader,strlen(EndOfHeader),0,TString::kExact);
   if(pBuffer>=0)
      buffer.Remove(0,pBuffer+strlen(EndOfHeader)+1);
   return;
}
