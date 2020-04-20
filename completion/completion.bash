_eh_complete() {
  COMPREPLY=()
  local word="${COMP_WORDS[COMP_CWORD]}"
  local completions="$(exec-helper --auto-complete "$word")"
  COMPREPLY=( $(compgen -W "$completions" -- "$word") )
}

complete -f -F _eh_complete exec-helper
complete -f -F _eh_complete eh
