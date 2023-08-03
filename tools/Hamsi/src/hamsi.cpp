/**
 * @file    hamsi.cpp
 * @author  Enes AYDIN (enes.aydin@daiichi.com)
 * @brief   Implementation of the Hamsi
 * @version 0.1
 * @date    2021-08-18
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include "dcs_rules.hpp"
#include "hamsi.hpp"

#include "clang/AST/DeclBase.h"
#include "clang/AST/Redeclarable.h"

#include "clang/Basic/FileManager.h"
#include "clang/Basic/Diagnostic.h"
#include "clang/Basic/SourceManagerInternals.h"
#include "clang/Basic/TargetInfo.h"
#include "clang/Basic/TargetOptions.h"

#include "clang/Lex/PPCallbacks.h"
#include "clang/Lex/Preprocessor.h"
#include "clang/Lex/PreprocessorOptions.h"

#include "clang/Parse/ParseAST.h"

#include "clang/Rewrite/Core/Rewriter.h"
#include "clang/Rewrite/Frontend/Rewriters.h"

#include "llvm/Support/Host.h"
#include "llvm/Support/raw_ostream.h"

#include <iostream>
#include <vector>
#include <string>
#include <regex>

using namespace clang;

template <typename T, typename U, typename V>
static std::string hamsi_ConvertString(T t, U u, V v)
{
    return Lexer::getSourceText(CharSourceRange::getTokenRange(t->getBeginLoc(), t->getEndLoc()), *u, *v).str();
}

static std::vector<std::string> hamsi_StringToVectorByLine(std::string str)
{
    std::string line;
    std::stringstream stream(str);
    std::vector<std::string> lines;

    while (std::getline(stream, line))
    {
        lines.push_back(line);
    }
    
    return lines;
}

bool CustomASTVisitor::VisitEnumDecl(EnumDecl *p_enumDecl)
{
    if (p_sourceManager->isInSystemHeader(p_enumDecl->getBeginLoc()))
    {
        return true;
    }
    
    dcs_setfilenamewithpath(p_sourceManager->getFilename(p_enumDecl->getBeginLoc()).str());

    auto enum_start_line = p_sourceManager->getExpansionLineNumber(p_enumDecl->getBeginLoc());
    auto enum_end_line = p_sourceManager->getExpansionLineNumber(p_enumDecl->getEndLoc());
    
    if (enum_start_line != enum_end_line)
    {
        bool diagnos_mes = false;
        auto lines = hamsi_StringToVectorByLine(hamsi_ConvertString(p_enumDecl, p_sourceManager, p_langOptions));

        if (lines.size() > 3U)
        {
            if (lines[1].find_first_of("{") == std::string::npos)
            {
                diagnos_mes = true;
            }
            else if (lines[lines.size() - 2U].find_first_of(",") != std::string::npos)
            {
                diagnos_mes = true;
            }
            else if (lines[lines.size() - 1U].find_first_of("}") == std::string::npos)
            {
                diagnos_mes = true;
            }
        }
        else
        {
            diagnos_mes = true;
        }

        if (diagnos_mes)
        {
            dcs_rule_print(DCS_RULE::DCS_4_5, enum_start_line);
        }
    }

    return true;
}

bool CustomASTVisitor::VisitRecordDecl(RecordDecl *p_recordDecl)
{
    if (p_sourceManager->isInSystemHeader(p_recordDecl->getBeginLoc()))
    {
        return true;
    }
    
    dcs_setfilenamewithpath(p_sourceManager->getFilename(p_recordDecl->getBeginLoc()).str());
    
    bool diagnos_mes = false;
    auto strct_start_line = p_sourceManager->getExpansionLineNumber(p_recordDecl->getBeginLoc());
    auto strct_end_line = p_sourceManager->getExpansionLineNumber(p_recordDecl->getEndLoc());
    
    if (strct_start_line != strct_end_line)
    {
        auto lines = hamsi_StringToVectorByLine(hamsi_ConvertString(p_recordDecl, p_sourceManager, p_langOptions));

        if (lines.size() > 3U)
        {
            if (lines[1].find_first_of("{") == std::string::npos)
            {
                diagnos_mes = true;
            }
            else if (lines[lines.size() - 2U].find_first_not_of(" \r") != std::string::npos)
            {
                diagnos_mes = true;
            }
            else if (lines[lines.size() - 1U].find_first_of("}") == std::string::npos)
            {
                diagnos_mes = true;
            }
        }
        else
        {
            diagnos_mes = true;
        }
    }

    if (diagnos_mes)
    {
        dcs_rule_print(DCS_RULE::DCS_4_6, strct_start_line);
    }

    return true;
}

bool CustomASTVisitor::VisitFunctionDecl(FunctionDecl *p_functionDecl)
{
    if (p_sourceManager->isInSystemHeader(p_functionDecl->getBeginLoc()))
    {
        return true;
    }

    dcs_setfilenamewithpath(p_sourceManager->getFilename(p_functionDecl->getBeginLoc()).str());

    unsigned int func_start_line = p_sourceManager->getExpansionLineNumber(p_functionDecl->getBeginLoc());

    if (p_functionDecl->hasBody())
    {
        for (auto *p_child_stmt : p_functionDecl->getBody()->children())
        {
            if (Stmt::StmtClass::IfStmtClass == p_child_stmt->getStmtClass())
            {
                IfStmt *p_ifStmt = static_cast<IfStmt*>(p_child_stmt);
                auto str_ifCond = hamsi_ConvertString(p_ifStmt->getCond(), p_sourceManager, p_langOptions);

                int cnt = 0;
                for (unsigned int i = 0; i < p_functionDecl->getNumParams(); ++i)
                {
                    if (str_ifCond.find(p_functionDecl->getParamDecl(i)->getNameAsString()) != std::string::npos)
                    {
                        cnt++;
                    }
                }
                
                if (cnt >= 2)
                {
                    dcs_rule_print(DCS_RULE::DCS_1_3, p_sourceManager->getExpansionLineNumber(p_ifStmt->getBeginLoc()));
                }
            }
        }
        
        auto lines = hamsi_StringToVectorByLine(hamsi_ConvertString(p_functionDecl, p_sourceManager, p_langOptions));
        
        for (std::size_t i = 0U; i < lines.size(); ++i)
        {
            if (auto ind = lines[i].find("/*"); ind != std::string::npos)
            {
                dcs_rule_print(DCS_RULE::DCS_7_2, func_start_line + i);
            }
        }
    }

    unsigned int func_end_line = p_functionDecl->hasBody() 
        ? p_sourceManager->getExpansionLineNumber(p_functionDecl->getBody()->getBeginLoc()) - 1U
        : p_sourceManager->getExpansionLineNumber(p_functionDecl->getEndLoc());
    
    if (func_start_line != func_end_line)
    {
        bool diagnos_mes = false;
        unsigned int first_param_line;
        unsigned int first_param_coll;

        if (p_functionDecl->getNumParams())
        {
            first_param_line = p_sourceManager->getExpansionLineNumber(p_functionDecl->getParamDecl(0)->getBeginLoc());
            first_param_coll = p_sourceManager->getExpansionColumnNumber(p_functionDecl->getParamDecl(0)->getBeginLoc());
        }

        for (unsigned int i = 1U; i < p_functionDecl->getNumParams(); ++i)
        { 
            auto start_coll = p_sourceManager->getExpansionColumnNumber(p_functionDecl->getParamDecl(i)->getBeginLoc());

            if (first_param_coll != start_coll)
            {
                diagnos_mes = true;
                break;
            }
        }

        if ((false == diagnos_mes) && (first_param_line != func_start_line) && (first_param_coll != 5U))
        {
            diagnos_mes = true;
        }

        if (diagnos_mes)
        {
            dcs_rule_print(DCS_RULE::DCS_3_2, func_start_line);
        }
    }
    
    return true;
}

bool CustomASTVisitor::VisitVarDecl(clang::VarDecl *p_varDecl)
{
    auto *p_init = p_varDecl->getAnyInitializer();

    if ((nullptr == p_init) || (p_sourceManager->isInSystemHeader(p_varDecl->getBeginLoc())))
    {
        return true;
    }

    auto lines = hamsi_StringToVectorByLine(hamsi_ConvertString(p_init, p_sourceManager, p_langOptions));

    if (lines[0].find_first_of("{") == std::string::npos)
    {
        return true;
    }

    dcs_setfilenamewithpath(p_sourceManager->getFilename(p_varDecl->getBeginLoc()).str());

    auto var_start_line = p_sourceManager->getExpansionLineNumber(p_varDecl->getBeginLoc());
    auto init_start_line = p_sourceManager->getExpansionLineNumber(p_init->getBeginLoc());
    auto init_end_line = p_sourceManager->getExpansionLineNumber(p_init->getEndLoc());
    
    if (init_end_line != var_start_line)
    {
        if ((init_start_line != var_start_line) || (lines[1].find_first_not_of(" \r") != std::string::npos))
        {
            dcs_rule_print(DCS_RULE::DCS_4_7, init_start_line);
        }
        else if (lines[lines.size() - 2U].find_first_of(",") == std::string::npos)
        {
            dcs_rule_print(DCS_RULE::DCS_5_1, init_start_line);
        }
    }

    return true;
}

bool CustomASTVisitor::VisitIfStmt(IfStmt *p_ifStmt)
{
    if (p_sourceManager->isInSystemHeader(p_ifStmt->getBeginLoc()))
    {
        return true;
    }

    dcs_setfilenamewithpath(p_sourceManager->getFilename(p_ifStmt->getBeginLoc()).str());
    
    auto p_ifCond = p_ifStmt->getCond();

    auto exp_start_line = p_sourceManager->getExpansionLineNumber(p_ifCond->getBeginLoc());
    auto exp_end_line = p_sourceManager->getExpansionLineNumber(p_ifCond->getEndLoc());
    auto exp_start_col = p_sourceManager->getExpansionColumnNumber(p_ifCond->getBeginLoc());

    auto str_exp = hamsi_ConvertString(p_ifCond, p_sourceManager, p_langOptions);
        
    if (exp_start_line != exp_end_line)
    {
        bool diagnos_mes = false;

        if (true == std::regex_search(str_exp, std::regex("[\\(\\w\\d]+ [^ \\w\\d]+ [\\w\\d\\)]+ [^ \\w\\d]+ [\\(\\w\\d]+ [^ \\w\\d]+ [\\w\\d\\)]+")))
        {
            diagnos_mes = true;
        }
        else if (true == std::regex_search(str_exp, std::regex("  [^ \\w\\d]+ [\\(\\w\\d]+ [^ \\w\\d]+")))
        {
            diagnos_mes = true;
        }
        else
        {
            std::string::size_type finding_col{exp_start_col};

            while ((finding_col = str_exp.find_first_of("\n", finding_col + 1U)) != std::string::npos)
            {
                if (exp_start_col != (str_exp.find_first_not_of(" \r\n", finding_col + 1U) - finding_col))
                {
                    diagnos_mes = true;
                    break;
                }
            }
        }

        if (diagnos_mes)
        {
            dcs_rule_print(DCS_RULE::DCS_3_5, exp_start_line);
        }
    }

    return true;
}

bool CustomASTVisitor::VisitSwitchStmt(SwitchStmt *p_switch)
{
    if (p_sourceManager->isInSystemHeader(p_switch->getBeginLoc()))
    {
        return true;
    }
    
    dcs_setfilenamewithpath(p_sourceManager->getFilename(p_switch->getBeginLoc()).str());
    
    std::size_t ind{};
    bool diagnos_mes = false;

    auto lines = hamsi_StringToVectorByLine(hamsi_ConvertString(p_switch, p_sourceManager, p_langOptions));

    while ((ind + 1U) < lines.size())
    {
        if ((true == std::regex_search(lines[ind], std::regex("switch"))) && 
            (lines[ind + 1U].find_first_of("{") == std::string::npos))
        {
            diagnos_mes = true;
            break;
        }
        else if ((true == std::regex_search(lines[ind], std::regex("case"))) && 
                    ((lines[ind - 1U].find_first_not_of(" \r}{") != std::string::npos) ||
                    (lines[ind + 1U].find_first_not_of(" \r{") != std::string::npos)))
        {
            diagnos_mes = true;
            break;
        }
        else if ((true == std::regex_search(lines[ind], std::regex("break"))) && 
            ((lines[ind - 1U].find_first_not_of(" \r") != std::string::npos) ||
            (lines[ind + 1U].find_first_not_of(" \r}") != std::string::npos)))
        {
            diagnos_mes = true;
            break;
        }
        else if ((true == std::regex_search(lines[ind], std::regex("default"))) && 
            ((lines[ind - 1U].find_first_not_of(" \r}") != std::string::npos) ||
            (lines[ind + 1U].find_first_not_of(" \r{") != std::string::npos)))
        {
            diagnos_mes = true;
            break;
        }

        ind++;
    }

    if (diagnos_mes)
    {
        dcs_rule_print(DCS_RULE::DCS_4_4, p_sourceManager->getExpansionLineNumber(p_switch->getBeginLoc()));
    }

    return true;
}

bool CustomASTVisitor::VisitCompoundStmt(CompoundStmt *p_compoundStmt)
{
    if (p_sourceManager->isInSystemHeader(p_compoundStmt->getBeginLoc()))
    {
        return true;
    }
    
    dcs_setfilenamewithpath(p_sourceManager->getFilename(p_compoundStmt->getBeginLoc()).str());
    
    bool diagnos_mes = false;

    auto lines = hamsi_StringToVectorByLine(hamsi_ConvertString(p_compoundStmt, p_sourceManager, p_langOptions));

    if (lines.size() > 2U)
    {
        if (lines[1].find_first_not_of(" \r\n") == std::string::npos)
        {
            diagnos_mes = true;
        }
        else if (lines[lines.size() - 2U].find_first_not_of(" \r\n") == std::string::npos)
        {
            diagnos_mes = true;
        }

        if (diagnos_mes)
        {
            dcs_rule_print(DCS_RULE::DCS_4_3, p_sourceManager->getExpansionLineNumber(p_compoundStmt->getBeginLoc()));
        }
    }

    return true;
}

bool CustomASTVisitor::VisitStmt(Stmt *p_stmt)
{
    if (p_sourceManager->isInSystemHeader(p_stmt->getBeginLoc()))
    {
        return true;
    }
    
    dcs_setfilenamewithpath(p_sourceManager->getFilename(p_stmt->getBeginLoc()).str());
    
    if (isa<ConditionalOperator>(p_stmt))
    {
        bool diagnos_mes = false;
        ConditionalOperator *p_condOpt = static_cast<ConditionalOperator*>(p_stmt);

        auto cond_start_line = p_sourceManager->getExpansionLineNumber(p_condOpt->getBeginLoc());
        auto cond_end_line = p_sourceManager->getExpansionLineNumber(p_condOpt->getEndLoc());
        auto cond_start_col = p_sourceManager->getExpansionColumnNumber(p_condOpt->getBeginLoc());

        if ((cond_start_line + 2U) == cond_end_line)
        {
            auto str_cond = hamsi_ConvertString(p_condOpt, p_sourceManager, p_langOptions);

            if (false == std::regex_search(str_cond, std::regex("\\n[ ]*\\? [^ ]")))
            {
                diagnos_mes = true;
            }
            else if (false == std::regex_search(str_cond, std::regex("\\n[ ]*: [^ ]")))
            {
                diagnos_mes = true;
            }
            else
            {
                auto newline_ind = str_cond.find("\n");
                auto question_ind = str_cond.find("?");
                auto second_newline_ind = str_cond.find("\n", question_ind);
                auto colon_ind = str_cond.find(":");

                if ((newline_ind == std::string::npos) || 
                    (question_ind == std::string::npos) || 
                    (second_newline_ind == std::string::npos) || 
                    (colon_ind == std::string::npos))
                {
                    diagnos_mes = true;
                }
                else if ((cond_start_col != (question_ind - newline_ind)) || (cond_start_col != (colon_ind - second_newline_ind)))
                {
                    diagnos_mes = true;
                }
            }
        }
        else if (cond_start_line != cond_end_line)
        {
            diagnos_mes = true;
        }

        if (diagnos_mes)
        {
            dcs_rule_print(DCS_RULE::DCS_3_1, cond_start_line);
        }
    }
    else if (isa<BinaryOperator>(p_stmt) && static_cast<BinaryOperator*>(p_stmt)->isAssignmentOp())
    {
        BinaryOperator *p_binaryOpt = static_cast<BinaryOperator*>(p_stmt);

        auto bin_start_line = p_sourceManager->getExpansionLineNumber(p_binaryOpt->getBeginLoc());
        auto bin_end_line = p_sourceManager->getExpansionLineNumber(p_binaryOpt->getEndLoc());

        if (bin_start_line != bin_end_line)
        {
            auto str_bin = hamsi_ConvertString(p_binaryOpt, p_sourceManager, p_langOptions);

            if (str_bin.find("?") == std::string::npos) // is not ConditionalOperator
            {
                bool diagnos_mes = false;
                auto bin_start_col = p_sourceManager->getExpansionColumnNumber(p_binaryOpt->getBeginLoc());

                if (auto assign_opt = str_bin.find(" = ") + 3U; str_bin.find_first_of("+-/*&|%", assign_opt) == std::string::npos)
                {
                    if (false == std::regex_search(str_bin, std::regex(" = {0,5}\\r?\\n")))
                    {
                        diagnos_mes = true;
                    }
                    else if (auto newline_ind = str_bin.find("\n"); (bin_start_col + 4U) != str_bin.find_first_not_of(" ", newline_ind + 1U) - newline_ind)
                    {
                        diagnos_mes = true;
                    }

                    if (diagnos_mes)
                    {
                        dcs_rule_print(DCS_RULE::DCS_3_3, bin_start_line);
                    }
                }
                else
                {
                    if (true == std::regex_search(str_bin, std::regex("[\\+-/\\*&\\|%] *[\\w]")))
                    {
                        diagnos_mes = true;
                    }
                    else
                    {
                        auto align_col = bin_start_col + assign_opt;
                        std::string::size_type finding_col{assign_opt};

                        while ((finding_col = str_bin.find_first_of("+-/*&|%", finding_col + 1U)) != std::string::npos)
                        {
                            if (align_col != (str_bin.find_first_not_of(" \r\n", finding_col + 1U) - str_bin.find("\n", finding_col)))
                            {
                                diagnos_mes = true;
                                break;
                            }
                        }
                    }

                    if (diagnos_mes)
                    {
                        dcs_rule_print(DCS_RULE::DCS_3_4, bin_start_line);
                    }
                }
            }
        }
    }

    return true;
}

class HeaderIncludesCallback : public PPCallbacks {
    std::string main_file_name;
    SourceManager *p_sourceManager;
public:
    HeaderIncludesCallback(const std::string &file_name, SourceManager &p_sM): main_file_name(file_name), p_sourceManager(&p_sM){}

    virtual void InclusionDirective(SourceLocation HashLoc,
                                    const Token &IncludeTok,
                                    StringRef FileName,
                                    bool IsAngled,
                                    CharSourceRange FilenameRange,
                                    const FileEntry *File,
                                    StringRef SearchPath,
                                    StringRef RelativePath,
                                    const Module *Imported,
                                    SrcMgr::CharacteristicKind FileType)
    {
        (void)IncludeTok;
        (void)FileName;
        (void)FilenameRange;
        (void)File;
        (void)SearchPath;
        (void)RelativePath;
        (void)Imported;

        if (main_file_name == p_sourceManager->getFilename(HashLoc).str())
        {
            if ((IsAngled && !isSystem(FileType)) || (!IsAngled && isSystem(FileType)))
            {
                dcs_setfilenamewithpath(main_file_name);
                dcs_rule_print(DCS_RULE::DCS_6_2, p_sourceManager->getExpansionLineNumber(HashLoc));
            }
        }
    }
};

Hamsi::Hamsi(int argc, char *argv[])
{
    (void)argc;
    // llvm::cl::ParseCommandLineOptions(argc, argv, "My simple front end\n");
    // CompilerInstance will hold the instance of the Clang compiler for us,
    // managing the various objects needed to run the compiler
    
    // Diagnostics manage problems and issues in compile
    compilerInstance.createDiagnostics(NULL, false);

    // Set target platform options
    // Initialize target info with the default triple for our platform
    std::shared_ptr<TargetOptions> TO = std::make_shared<TargetOptions>();
    TO->Triple = llvm::sys::getDefaultTargetTriple();
    TargetInfo *TI = TargetInfo::CreateTargetInfo(compilerInstance.getDiagnostics(), TO);
    compilerInstance.setTarget(TI);

    compilerInstance.getLangOpts().C99 = true;
    compilerInstance.getLangOpts().CommentOpts.ParseAllComments=true;
    // FileManager supports for file system lookup, file system caching, and directory search management
    compilerInstance.createFileManager();

    // SourceManager handles loading and caching of source files into memory.
    compilerInstance.createSourceManager(compilerInstance.getFileManager());

    compilerInstance.getHeaderSearchOpts().AddPath("..\\..\\..\\f7-base-radio-toolchain\\10.2.1\\arm-none-eabi\\include", frontend::IncludeDirGroup::CSystem, false, true);
    compilerInstance.getHeaderSearchOpts().AddPath("..\\..\\..\\f7-base-radio-toolchain\\10.2.1\\lib\\gcc\\arm-none-eabi\\10.2.1\\include", frontend::IncludeDirGroup::CSystem, false, true);

    // Prreprocessor runs within
    compilerInstance.createPreprocessor(TU_Prefix);
    
    // std::shared_ptr<PreprocessorOptions> PPOpts = std::make_shared<PreprocessorOptions>();
    // LangOptions languageOptions;
    // std::shared_ptr<HeaderSearchOptions> headerSearchOptions = std::make_shared<HeaderSearchOptions>();
    // HeaderSearch headerSearch(headerSearchOptions, compilerInstance.getSourceManager(), compilerInstance.getDiagnostics(), languageOptions, TI);
    // std::shared_ptr<Preprocessor> preprocessor = std::make_shared<Preprocessor>(PPOpts, compilerInstance.getDiagnostics(), languageOptions, compilerInstance.getSourceManager(), headerSearch, TheModuleLoader, nullptr, false, TU_Complete);

    //* CI.getPreprocessorOpts().UsePredefines = false;

    // ASTContext holds long‐lived AST nodes (such as types and decls) .
    // std::unique_ptr< clang::ASTConsumer > astConsumer = clang::CreateASTPrinter(NULL, "");
    // compilerInstance.setASTConsumer(std::move(astConsumer));
    compilerInstance.createASTContext();
    
    // A Rewriter helps us manage the code rewriting task.
    Rewriter TheRewriter;
    TheRewriter.setSourceMgr(compilerInstance.getSourceManager(), compilerInstance.getLangOpts());

    // Set the main file handled by the source manager to the input file.
    llvm::ErrorOr<const FileEntry *> FileIn = compilerInstance.getFileManager().getFile(argv[1]);

    dcs_setfilenamewithpath(argv[1]);
    
    auto file_id = compilerInstance.getSourceManager().createFileID(FileIn.get(), SourceLocation(), SrcMgr::C_User);
    compilerInstance.getSourceManager().setMainFileID(file_id);

    // std::cout << "getDiagnosticClient...\n";
    // Inform Diagnostics that processing of a source file is beginning.
    compilerInstance.getDiagnosticClient().BeginSourceFile(compilerInstance.getLangOpts(), &compilerInstance.getPreprocessor());

    compilerInstance.getPreprocessor().addPPCallbacks(std::make_unique<HeaderIncludesCallback>(FileIn.get()->getName().str(), compilerInstance.getSourceManager()));

    customConsumer.customVisitor.SetSourceManager(compilerInstance.getSourceManager());
    customConsumer.customVisitor.SetLangOptions(compilerInstance.getLangOpts());

    // Create an AST consumer instance which is going to get called by ParseAST.
    // Parse the file to AST, registering our consumer as the AST consumer.
    ParseAST(compilerInstance.getPreprocessor(), &customConsumer, compilerInstance.getASTContext());
}
