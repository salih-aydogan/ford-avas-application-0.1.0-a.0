# ------------------------------------------------
# Generic Makefile (based on gcc)
#
# ChangeLog :
#   2021-08-20 - first version
# ------------------------------------------------

######################################
# target
######################################
TARGET = hamsi

BUILD_DIR = build

HAMSI_SOURCES = \
src/hamsi.cpp \
src/dcs_rules.cpp 

HAMSI_INCLUDES =  \
-Isrc \

SOURCES = main.cpp \
		  $(HAMSI_SOURCES)


CLANG_LIBS = -lclangTooling -lclangFrontendTool -lclangFrontend -lclangDriver -lclangSerialization -lclangCodeGen -lclangParse -lclangSema -lclangStaticAnalyzerFrontend -lclangStaticAnalyzerCheckers -lclangStaticAnalyzerCore -lclangAnalysis -lclangARCMigrate -lclangRewrite -lclangRewriteFrontend -lclangEdit -lclangAST -lclangLex -lclangBasic -lclang
LLVM_CXXFLAGS = `llvm-config --cxxflags --ldflags`
LLVM_LDFLAGS = `llvm-config --libs --system-libs`

CXX = g++

CXXFLAGS = $(HAMSI_INCLUDES) -std=c++17 -Wall -Wextra -Wconversion -fexceptions

all:
	$(CXX) $(LLVM_CXXFLAGS) $(SOURCES) -o $(BUILD_DIR)/$(TARGET).exe $(CXXFLAGS) $(CLANG_LIBS) $(LLVM_LDFLAGS) -lversion

clean:
	cd $(BUILD_DIR) && rm * && cd ..

# CXXFLAGS := -fno-rtti -O0 -g
# CLANG_LIBS := \
# 	-Wl,--start-group \
# 	-lclangAST \
# 	-lclangASTMatchers \
# 	-lclangAnalysis \
# 	-lclangBasic \
# 	-lclangDriver \
# 	-lclangEdit \
# 	-lclangFrontend \
# 	-lclangFrontendTool \
# 	-lclangLex \
# 	-lclangParse \
# 	-lclangSema \
# 	-lclangEdit \
# 	-lclangRewrite \
# 	-lclangRewriteFrontend \
# 	-lclangStaticAnalyzerFrontend \
# 	-lclangStaticAnalyzerCheckers \
# 	-lclangStaticAnalyzerCore \
# 	-lclangCrossTU \
# 	-lclangIndex \
# 	-lclangSerialization \
# 	-lclangToolingCore \
# 	-lclangTooling \
# 	-lclangFormat \
# 	-Wl,--end-group
