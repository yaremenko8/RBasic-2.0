##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Debug
ProjectName            :=RBasic_2p0
ConfigurationName      :=Debug
WorkspacePath          :=/home/grigory/Documents
ProjectPath            :=/home/grigory/Documents/RBasic_2p0
IntermediateDirectory  :=./Debug
OutDir                 := $(IntermediateDirectory)
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
User                   :=grigory
Date                   :=05/23/18
CodeLitePath           :=/home/grigory/.codelite
LinkerName             :=g++
SharedObjectLinkerName :=g++ -shared -fPIC
ObjectSuffix           :=.o
DependSuffix           :=.o.d
PreprocessSuffix       :=.o.i
DebugSwitch            :=-gstab
IncludeSwitch          :=-I
LibrarySwitch          :=-l
OutputSwitch           :=-o 
LibraryPathSwitch      :=-L
PreprocessorSwitch     :=-D
SourceSwitch           :=-c 
OutputFile             :=$(IntermediateDirectory)/$(ProjectName)
Preprocessors          :=
ObjectSwitch           :=-o 
ArchiveOutputSwitch    := 
PreprocessOnlySwitch   :=-E 
ObjectsFileList        :="RBasic_2p0.txt"
PCHCompileFlags        :=
MakeDirCommand         :=mkdir -p
LinkOptions            :=  
IncludePath            :=  $(IncludeSwitch). $(IncludeSwitch). 
IncludePCH             := 
RcIncludePath          := 
Libs                   := 
ArLibs                 :=  
LibPath                := $(LibraryPathSwitch). 

##
## Common variables
## AR, CXX, CC, AS, CXXFLAGS and CFLAGS can be overriden using an environment variables
##
AR       := ar rcus
CXX      := g++
CC       := gcc
CXXFLAGS :=  -g -O0 -Wall $(Preprocessors)
CFLAGS   :=  -g -O0 -Wall $(Preprocessors)
ASFLAGS  := 
AS       := as


##
## User defined environment variables
##
CodeLiteDir:=/usr/share/codelite
Objects0=$(IntermediateDirectory)/builtins.cpp$(ObjectSuffix) $(IntermediateDirectory)/lexan.cpp$(ObjectSuffix) $(IntermediateDirectory)/main.cpp$(ObjectSuffix) $(IntermediateDirectory)/vars.cpp$(ObjectSuffix) $(IntermediateDirectory)/rbexcept.cpp$(ObjectSuffix) $(IntermediateDirectory)/seman.cpp$(ObjectSuffix) $(IntermediateDirectory)/exec.cpp$(ObjectSuffix) 



Objects=$(Objects0) 

##
## Main Build Targets 
##
.PHONY: all clean PreBuild PrePreBuild PostBuild MakeIntermediateDirs
all: $(OutputFile)

$(OutputFile): $(IntermediateDirectory)/.d $(Objects) 
	@$(MakeDirCommand) $(@D)
	@echo "" > $(IntermediateDirectory)/.d
	@echo $(Objects0)  > $(ObjectsFileList)
	$(LinkerName) $(OutputSwitch)$(OutputFile) @$(ObjectsFileList) $(LibPath) $(Libs) $(LinkOptions)

MakeIntermediateDirs:
	@test -d ./Debug || $(MakeDirCommand) ./Debug


$(IntermediateDirectory)/.d:
	@test -d ./Debug || $(MakeDirCommand) ./Debug

PreBuild:


##
## Objects
##
$(IntermediateDirectory)/builtins.cpp$(ObjectSuffix): builtins.cpp $(IntermediateDirectory)/builtins.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/grigory/Documents/RBasic_2p0/builtins.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/builtins.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/builtins.cpp$(DependSuffix): builtins.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/builtins.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/builtins.cpp$(DependSuffix) -MM builtins.cpp

$(IntermediateDirectory)/builtins.cpp$(PreprocessSuffix): builtins.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/builtins.cpp$(PreprocessSuffix) builtins.cpp

$(IntermediateDirectory)/lexan.cpp$(ObjectSuffix): lexan.cpp $(IntermediateDirectory)/lexan.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/grigory/Documents/RBasic_2p0/lexan.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/lexan.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/lexan.cpp$(DependSuffix): lexan.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/lexan.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/lexan.cpp$(DependSuffix) -MM lexan.cpp

$(IntermediateDirectory)/lexan.cpp$(PreprocessSuffix): lexan.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/lexan.cpp$(PreprocessSuffix) lexan.cpp

$(IntermediateDirectory)/main.cpp$(ObjectSuffix): main.cpp $(IntermediateDirectory)/main.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/grigory/Documents/RBasic_2p0/main.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/main.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/main.cpp$(DependSuffix): main.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/main.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/main.cpp$(DependSuffix) -MM main.cpp

$(IntermediateDirectory)/main.cpp$(PreprocessSuffix): main.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/main.cpp$(PreprocessSuffix) main.cpp

$(IntermediateDirectory)/vars.cpp$(ObjectSuffix): vars.cpp $(IntermediateDirectory)/vars.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/grigory/Documents/RBasic_2p0/vars.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/vars.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/vars.cpp$(DependSuffix): vars.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/vars.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/vars.cpp$(DependSuffix) -MM vars.cpp

$(IntermediateDirectory)/vars.cpp$(PreprocessSuffix): vars.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/vars.cpp$(PreprocessSuffix) vars.cpp

$(IntermediateDirectory)/rbexcept.cpp$(ObjectSuffix): rbexcept.cpp $(IntermediateDirectory)/rbexcept.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/grigory/Documents/RBasic_2p0/rbexcept.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/rbexcept.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/rbexcept.cpp$(DependSuffix): rbexcept.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/rbexcept.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/rbexcept.cpp$(DependSuffix) -MM rbexcept.cpp

$(IntermediateDirectory)/rbexcept.cpp$(PreprocessSuffix): rbexcept.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/rbexcept.cpp$(PreprocessSuffix) rbexcept.cpp

$(IntermediateDirectory)/seman.cpp$(ObjectSuffix): seman.cpp $(IntermediateDirectory)/seman.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/grigory/Documents/RBasic_2p0/seman.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/seman.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/seman.cpp$(DependSuffix): seman.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/seman.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/seman.cpp$(DependSuffix) -MM seman.cpp

$(IntermediateDirectory)/seman.cpp$(PreprocessSuffix): seman.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/seman.cpp$(PreprocessSuffix) seman.cpp

$(IntermediateDirectory)/exec.cpp$(ObjectSuffix): exec.cpp $(IntermediateDirectory)/exec.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/grigory/Documents/RBasic_2p0/exec.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/exec.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/exec.cpp$(DependSuffix): exec.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/exec.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/exec.cpp$(DependSuffix) -MM exec.cpp

$(IntermediateDirectory)/exec.cpp$(PreprocessSuffix): exec.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/exec.cpp$(PreprocessSuffix) exec.cpp


-include $(IntermediateDirectory)/*$(DependSuffix)
##
## Clean
##
clean:
	$(RM) -r ./Debug/


