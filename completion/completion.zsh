# completion.zsh

_eh_complete() {
  local word completions
  word="$1"
  completions="$(exec-helper --auto-complete "${word}")"
  reply=( "${(ps:\n:)completions}" )
}

compctl -f -K _eh_complete exec-helper
compctl -f -K _eh_complete eh
