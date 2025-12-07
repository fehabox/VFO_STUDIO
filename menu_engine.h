/*
    Project: VFO Studio V1.0
    fehabox Software License
    Copyright © fehabox.com
    Author: Femi Hasani (@fehabox), ex YU8CF

    License   : License_Details.txt
    Credits   : credits.txt
    Features  : VFO_Studio_Features.txt
    GitHub    : https://github.com/fehabox
*/
#pragma once

// ===============================================
// UNIVERSAL MENU ENGINE – Working Serial Version
// ===============================================

struct MenuNode {
    const char* label;
    const char** items;
    int itemCount;
    void (*onSelect)(int index);
    const MenuNode* subMenus;
    int subMenuCount;

    int menuID;     // <--- ADD THIS
};

struct MenuState {
    const MenuNode* node;
    int index;          // -1 = BACK
    int scroll;
    int stackPos;
};

static MenuState menuStateStack[10];
static MenuState* s = nullptr;


// ===============================================
// INIT
// ===============================================
void menuInit(const MenuNode* root)
{
    menuStateStack[0].node     = root;
    menuStateStack[0].index    = -1;      // title selected first
    menuStateStack[0].scroll   = 0;
    menuStateStack[0].stackPos = 0;

    s = &menuStateStack[0];
}


// ===============================================
// GET STATE / NODE
// ===============================================
const MenuState& menuCurrentState() { return *s; }
const MenuNode*  menuCurrentNode()  { return s->node; }


// ===============================================
// MOVE CURSOR (UP/DOWN)
// ===============================================
void menuNavigate(int d)
{
    const MenuNode* node = s->node;

    int count = (node->subMenuCount > 0)
                ? node->subMenuCount
                : node->itemCount;

    if (count == 0) return;

    int old = s->index;

    if (s->index == -1 && d < 0) {
        s->index = count - 1;
        return;
    }

    s->index += d;

    if (s->index >= count) s->index = -1;
    if (s->index < -1)     s->index = count - 1;
}


// ===============================================
// SELECT (ENTER/BACK)
// ===============================================
void menuSelect()
{
    const MenuNode* node = s->node;

    // ------------------ BACK (title row) --------------------
    if (s->index == -1)
    {
        if (s->stackPos > 0)
        {
            s->stackPos--;
            s = &menuStateStack[s->stackPos];
        }
        return;
    }

    // ------------------ ENTER SUBMENU -----------------------
    if (node->subMenuCount > 0)
    {
        const MenuNode* child = &node->subMenus[s->index];

        if (child->subMenuCount > 0 || child->itemCount > 0)
        {
            int next = s->stackPos + 1;

            menuStateStack[next].node     = child;
            menuStateStack[next].index    = -1;
            menuStateStack[next].scroll   = 0;
            menuStateStack[next].stackPos = next;

            s = &menuStateStack[next];
            return;
        }
    }

    // ------------------ LEAF (CALLBACK) ---------------------
    if (node->onSelect)
        node->onSelect(s->index);
}
