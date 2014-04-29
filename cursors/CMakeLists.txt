set(cursors_folders
    Bridge
   )

foreach(theme ${cursors_folders})
    install(DIRECTORY ${theme} DESTINATION ${ICON_INSTALL_DIR})
endforeach(theme)

