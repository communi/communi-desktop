######################################################################
# Communi
######################################################################

exists(libs/backend/src/src.pri) {
    include(libs/backend/src/src.pri)
} else:load(communi-config, true) {
    # bool load(feature, ignore_error)
    CONFIG += communi
    COMMUNI += core model util
} else {
    error(Cannot detect libcommuni. Either install it or run \'git submodule update --init\' in $${_PRO_FILE_PWD_}.)
}

# TODO:
include(app/app.pri)
include(libs/base/base.pri)
include(libs/shared/shared.pri)
include(libs/3rdparty/3rdparty.pri)
