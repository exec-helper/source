# Use this script to add completions to your shell
#   Usage: Add
#      source "<full path to this script>/init-completion.sh"
#   to your profile or bashrc
#
# Note: package maintainers can install the completion files directly to the right folders in the OS (if supported by the OS) to enable auto discovery of the completion files.
if [ -n "$BASH_VERSION" ]; then
  root="$(dirname "${BASH_SOURCE[0]}")"
  source "$root/completion.bash"

elif [ -n "$ZSH_VERSION" ]; then
  root="$(dirname "$0")"
  source "$root/completion.zsh"
fi

