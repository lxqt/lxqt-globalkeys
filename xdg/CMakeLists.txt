# install default config files to /etc/xdg
install(FILES
    globalkeyshortcuts.conf
    DESTINATION "${LXQT_ETC_XDG_DIR}/lxqt"
    COMPONENT Runtime
)
