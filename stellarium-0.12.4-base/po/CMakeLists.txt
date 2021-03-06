# Special targets for translations:
#
# translations
#   Converts all PO files to GMO files. Note that it does *not* update
#   the PO files or the PO templates -- in fact, these files are never
#   updated automatically.
#
# generate-pot
#   Re-creates all POT files unconditionally.
#
# update-po
#   Updates all PO files unconditionally. Note that it takes care of
#   updating the POT files.
#
# translations-<DOMAIN>
# generate-pot-<DOMAIN>
# update-po-<DOMAIN>
#   Same as above, but only affect the files in the corresponding
#   po/<DOMAIN> directory. (DOMAIN is actually the base name of the POT
#   file in the subdirectory, but that should match the directory name
#   anyway.)

ADD_CUSTOM_TARGET(translations)
ADD_CUSTOM_TARGET(generate-pot)
ADD_CUSTOM_TARGET(update-po)

# GETTEXT_CREATE_TRANSLATIONS(domain [DEFAULT_TARGET] lang1 ... langN)
#
# Creates custom build rules to create and install (G)MO files for the
# specified languages. If the DEFAULT_TARGET option is used, the
# translations will also be created when building the default target.
#
# "domain" is the translation domain, eg. "stellarium". A POT file
# with the name ${domain}.pot must exist in the directory of the
# CMakeLists.txt file invoking the macro.
#
# This macro also creates the "translations-${domain}" and
# "update-po-${domain}" targets (see above for an explanation).
#
MACRO(GETTEXT_CREATE_TRANSLATIONS _domain _firstLang)

    SET(_gmoFiles)
    GET_FILENAME_COMPONENT(_absPotFile ${_domain}.pot ABSOLUTE)

    # Update these PO files when building the "update-po-<DOMAIN>" and
    # "update-po" targets.
    ADD_CUSTOM_TARGET(update-po-${_domain})
    ADD_DEPENDENCIES(update-po update-po-${_domain})

    # Make sure the POT file is updated before updating the PO files.
    ADD_DEPENDENCIES(update-po-${_domain} generate-pot-${_domain})

    SET(_addToAll)
    IF(${_firstLang} STREQUAL "DEFAULT_TARGET")
        SET(_addToAll "ALL")
        SET(_firstLang)
    ENDIF(${_firstLang} STREQUAL "DEFAULT_TARGET")

    FOREACH (_lang ${ARGN})
        GET_FILENAME_COMPONENT(_absFile ${_lang}.po ABSOLUTE)
        FILE(RELATIVE_PATH _relFile ${PROJECT_SOURCE_DIR} ${_absFile})
        SET(_gmoFile ${CMAKE_CURRENT_BINARY_DIR}/${_lang}.gmo)

        # Convert a PO file into a GMO file.
        ADD_CUSTOM_COMMAND(
            OUTPUT ${_gmoFile} 
            COMMAND ${GETTEXT_MSGFMT_EXECUTABLE} -o ${_gmoFile} ${_absFile}
            DEPENDS ${_absFile}
        )

        # Update the PO file unconditionally when building the
        # "update-po-<DOMAIN>" target. Note that to see the file being
        # processed, we have to run "cmake -E echo", because the
        # COMMENT is not displayed by cmake...
        ADD_CUSTOM_COMMAND(
            TARGET update-po-${_domain}
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E echo "** Updating ${_relFile}"
            COMMAND ${GETTEXT_MSGMERGE_EXECUTABLE}
                --quiet --update -m --backup=none -s
                ${_absFile} ${_absPotFile}
            VERBATIM
        )

        INSTALL(FILES ${_gmoFile} DESTINATION share/locale/${_lang}/LC_MESSAGES RENAME ${_domain}.mo)
        SET(_gmoFiles ${_gmoFiles} ${_gmoFile})

   ENDFOREACH (_lang)

   # Create the GMO files when building the "translations-<DOMAIN>" and
   # "translations" targets.
   ADD_CUSTOM_TARGET(translations-${_domain} ${_addToAll} DEPENDS ${_gmoFiles})
   ADD_DEPENDENCIES(translations translations-${_domain})

ENDMACRO(GETTEXT_CREATE_TRANSLATIONS )


ADD_SUBDIRECTORY(stellarium) 
ADD_SUBDIRECTORY(stellarium-skycultures) 
