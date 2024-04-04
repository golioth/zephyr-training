#!/bin/bash

west init -l app
west update
west zephyr-export
pip install -r deps/zephyr/scripts/requirements.txt
echo "alias ll='ls -lah'" >> $HOME/.bashrc
west completion bash > $HOME/west-completion.bash
echo 'source $HOME/west-completion.bash' >> $HOME/.bashrc
history -c
