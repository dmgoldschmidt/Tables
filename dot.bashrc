# .bashrc

# User specific aliases and functions

# Source global definitions
if [ -f /etc/bashrc ]; then
	. /etc/bashrc
fi
eval `dircolors`
#alias ls='ls -F --color '
PS1='\w($(hostname | ~/myhost)): '
alias big_emacs='emacs -geometry 90x40 -fn 9x15bold -bg white -fg black &'
alias big_xterm='xterm -geometry 90x40 -fn 9x15bold -bg white -fg black &'
alias xdvi='xdvi -s 6 -geometry 800x700+150+0 -sidemargin 1cm'
alias baldwin='ssh -l david baldwin.goldbell.org'
alias recent='ls -lt | head'
alias gdb='~/gdb-7.1/gdb/gdb'
export PATH=/opt/local/bin:/usr/local/bin:/usr/local/texlive/2012/bin/x86_64-darwin:$PATH
export MOZ_NO_REMOTE=1
export EDITOR=emacs

