// Copyright (c) 2005 - 2017 Settlers Freaks (sf-team at siedler25.org)
//
// This file is part of Return To The Roots.
//
// Return To The Roots is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// Return To The Roots is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Return To The Roots. If not, see <http://www.gnu.org/licenses/>.

#include "defines.h" // IWYU pragma: keep
#include <build_version.h>
#include "dskMainMenu.h"

#include "WindowManager.h"
#include "Loader.h"
#include "GlobalVars.h"

#include "Settings.h"

#include "desktops/dskSinglePlayer.h"
#include "desktops/dskMultiPlayer.h"
#include "desktops/dskOptions.h"
#include "desktops/dskIntro.h"
#include "desktops/dskCredits.h"
#include "controls/ctrlTimer.h"
#include "ingameWindows/iwMsgbox.h"
#include "ingameWindows/iwTextfile.h"
#include "ogl/glArchivItem_Font.h"

enum{
    ID_txtVersion,
    ID_txtURL,
    ID_txtCopyright,
    ID_btSingleplayer,
    ID_btMultiplayer,
    ID_btOptions,
    ID_btIntro,
    ID_btReadme,
    ID_btCredits,
    ID_btQuit,
    ID_logo,
    ID_tmrDebugData
};

dskMainMenu::dskMainMenu() : Desktop(LOADER.GetImageN("menu", 0))
{
    // Version
    AddVarText(ID_txtVersion, 0, 600, _("Return To The Roots - v%s-%s"), COLOR_YELLOW, 0 | glArchivItem_Font::DF_BOTTOM, NormalFont, 2, GetWindowVersion(), GetWindowRevisionShort());
    // URL
    AddText(ID_txtURL, 400, 600, _("http://www.siedler25.org"), COLOR_GREEN, glArchivItem_Font::DF_CENTER | glArchivItem_Font::DF_BOTTOM, NormalFont);
    // Copyright
    AddVarText(ID_txtCopyright, 800, 600, _("© 2005 - %s Settlers Freaks"), COLOR_YELLOW, glArchivItem_Font::DF_RIGHT | glArchivItem_Font::DF_BOTTOM, NormalFont, 1, GetCurrentYear());

    // "Einzelspieler"
    AddTextButton(ID_btSingleplayer, 115, 180, 220, 22, TC_GREEN2, _("Singleplayer"), NormalFont);
    // "Mehrspieler"
    AddTextButton(ID_btMultiplayer, 115, 210, 220, 22, TC_GREEN2, _("Multiplayer"), NormalFont);
    // "Optionen"
    AddTextButton(ID_btOptions, 115, 250, 220, 22, TC_GREEN2, _("Options"), NormalFont);
    // "Intro"
    AddTextButton(ID_btIntro, 115, 280, 220, 22, TC_GREEN2, _("Intro"), NormalFont);
    // "ReadMe"
    AddTextButton(ID_btReadme, 115, 310, 220, 22, TC_GREEN2, _("Readme"), NormalFont);
    // "Credits"
    AddTextButton(ID_btCredits, 115, 340, 220, 22, TC_GREEN2, _("Credits"), NormalFont);
    // "Programm verlassen"
    AddTextButton(ID_btQuit, 115, 390, 220, 22, TC_RED1, _("Quit program"), NormalFont);

    AddImage(ID_logo, 20, 20, LOADER.GetImageN("logo", 0));

    if (SETTINGS.global.submit_debug_data == 0)
        AddTimer(ID_tmrDebugData, 250);

    /*AddText(20, 50, 450, _("Font Test"), COLOR_YELLOW, glArchivItem_Font::DF_LEFT, SmallFont);
    AddText(21, 50, 470, _("Font Test"), COLOR_YELLOW, glArchivItem_Font::DF_LEFT, NormalFont);
    AddText(22, 50, 490, _("Font Test"), COLOR_YELLOW, glArchivItem_Font::DF_LEFT, LargeFont);*/
    //  !\"#$%&'()*+,-./0123456789:;<=>?@abcdefghijklmnopqrstuvwxyz\\_ABCDEFGHIJKLMNOPQRSTUVWXYZÇüéâäàåçêëèïîì©ÄÅôöòûùÖÜáíóúñ
}

void dskMainMenu::Msg_Timer(const unsigned int ctrl_id)
{
    GetCtrl<ctrlTimer>(ctrl_id)->Stop();
    WINDOWMANAGER.Show( new iwMsgbox(_("Submit debug data?"), _("RttR now supports sending debug data. Would you like to help us improving this game by sending debug data?"), this, MSB_YESNO, MSB_QUESTIONRED, 100) );
}

void dskMainMenu::Msg_MsgBoxResult(const unsigned msgbox_id, const MsgboxResult mbr)
{
    if (msgbox_id == 100)
    {
        if (mbr == MSR_YES)
            SETTINGS.global.submit_debug_data = 1;
        else
            SETTINGS.global.submit_debug_data = 2;

        SETTINGS.Save();
    }
}

void dskMainMenu::Msg_ButtonClick(const unsigned int ctrl_id)
{
    switch(ctrl_id)
    {
        case ID_btSingleplayer: // "Single Player"
            WINDOWMANAGER.Switch(new dskSinglePlayer);
            break;
        case ID_btMultiplayer: // "Multiplayer"
            WINDOWMANAGER.Switch(new dskMultiPlayer);
            break;
        case ID_btOptions: // "Options"
            WINDOWMANAGER.Switch(new dskOptions);
            break;
        case ID_btIntro: // "Intro"
            WINDOWMANAGER.Switch(new dskIntro);
            break;
        case ID_btCredits: // "Credits"
            WINDOWMANAGER.Switch(new dskCredits);
            break;
        case ID_btQuit: // "Quit"
            GLOBALVARS.notdone = false;
            break;
        case ID_btReadme: // "Readme"
            WINDOWMANAGER.Show(new iwTextfile("readme.txt", _("Readme!")));
            break;
    }
}
