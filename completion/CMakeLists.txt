install(FILES init-completion.sh completion.bash completion.zsh DESTINATION share/exec-helper/completions COMPONENT runtime)

install(FILES completion.bash RENAME eh DESTINATION share/bash-completion COMPONENT runtime)
install(FILES completion.bash RENAME exec-helper DESTINATION share/bash-completion COMPONENT runtime)

install(FILES _eh _exec-helper DESTINATION share/zsh/functions/Completion/Unix COMPONENT runtime)
