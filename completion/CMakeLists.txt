install(FILES init-completion.sh completion.bash completion.zsh DESTINATION share/exec-helper/completions COMPONENT runtime)

set(BASH_COMPLETION_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}/share/bash-completion" CACHE PATH "Installation prefix for bash completions")
install(FILES completion.bash RENAME eh DESTINATION "${BASH_COMPLETION_INSTALL_PREFIX}" COMPONENT runtime)
install(FILES completion.bash RENAME exec-helper DESTINATION "${BASH_COMPLETION_INSTALL_PREFIX}" COMPONENT runtime)

set(ZSH_COMPLETION_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}/share/zsh/functions/Completion/Unix" CACHE PATH "Installation prefix for zsh completions")
install(FILES _eh _exec-helper DESTINATION "${ZSH_COMPLETION_INSTALL_PREFIX}" COMPONENT runtime)
