#pragma once
//w = working.
//b = broken.

enum class CMDS {
    //SINGLE-FILE
    SIGN, //w
    UNSIGN, //w
    IMPORT, //w
    UPDATE, //w
    PUSH, //w
    RENAME, //w

    PURGE,
    DELETELIB,

    //DEBUG
    ALL, //w
    RCALC, //w
    FIND, //w
    CWD, //w
    CWDF, //w

    //GROUP
    ADDTOGROUP, //w
    IMPORTGROUP, //w
    UPDATEGROUP, //w
    ALLG, //w
    ALLIG, //w
    
    PURGEGROUP,
    DELETEGROUP,
    
    //CONFIG
    CONFIG, //w
    LISTCONFIG, //w

    UNK
};