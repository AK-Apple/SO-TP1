#!/bin/bash

alias dcrun="docker run -v ${PWD}:/root --privileged -ti --name Carancho agodio/itba-so:1.0"
alias dcexec="docker exec -ti Carancho bash"
alias dcstart="docker start Carancho"
alias dcstop="docker stop Carancho"
alias dcrm="docker rm -f Carancho"

# docker rm -f Carancho
# docker run -v ${PWD}:/root --privileged -ti --name Carancho agodio/itba-so:1.0

# alias ls='ls --color'

# cd root