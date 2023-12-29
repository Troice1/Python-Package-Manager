#pragma once
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include <d3d11.h>
#include <tchar.h>
#include <iostream>
#include <string>
#include <locale> 
#include <codecvt> 
#include <fstream>

namespace col {
	static int color = 1;
}

namespace message {
    static std::string packageList = "请点击上方获取"; // pip list
    static std::string UpdatableLibrary = "点击上方扫描按钮即可获取需要更新的库"; // pip list -o
    static char ModuleDetails[1024] = ""; // pip show [name]
    static char char_returnvalue[4096] = "";
    static char char_returnvalue_[4096] = "";
}

namespace InPut {
    static char name[1024] = "urllib";
    static char name_install[1024] = "";
    static char name_uninstall[1024] = "";
}

namespace FontColor {
    static ImVec4 FontColor_1 = { 1.0f,1.0f,0.0f,0.5f };
}

namespace TroiceFunction {
	std::string TBS(std::string& str)
	{
		typedef std::codecvt_byname<wchar_t, char, std::mbstate_t>F;
		static std::wstring_convert<F>strC(new F("Chinese"));
		static std::wstring_convert<std::codecvt_utf8<wchar_t>> strCnv;
		return strCnv.to_bytes(strC.from_bytes(str));
	}

    std::string getCmdResult(const std::string& strCmd)
    {
        char buf[10240] = { 0 };
        FILE* pf = NULL;

        if ((pf = _popen(strCmd.c_str(), "r")) == NULL)
        {
            return "";
        }

        std::string strResult;
        while (fgets(buf, sizeof buf, pf))
        {
            strResult += buf;
        }

        _pclose(pf);

        unsigned int iSize = strResult.size();
        if (iSize > 0 && strResult[iSize - 1] == '\n')  // linux
        {
            strResult = strResult.substr(0, iSize - 1);
        }

        return strResult;
    }

    static void HelpMarker(const char* desc)
    {
        ImGui::TextDisabled("(?)");
        if (ImGui::BeginItemTooltip())
        {
            ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
            ImGui::TextUnformatted(desc);
            ImGui::PopTextWrapPos();
            ImGui::EndTooltip();
        }
    }
}

