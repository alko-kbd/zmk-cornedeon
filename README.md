# zmk-cornedeon

ZMK-based handwired Cornedeon

Keyboard Maintainer: [alko](https://github.com/alko-kbd/) [alko-kbd@alk0.ru](mailto:alko-kbd@alk0.ru)

Web Site: [cornedeon.ru](https://cornedeon.ru)

![Cornedeon](pics/cornedeon_s.jpg)

## Local build

Prepare build environvent (devcontainer) as described in ZMK docs.

~# cd zmk-workspace/zmk

~zmk-workspace/zmk# git clone https://github.com/alko-kbd/zmk-cornedeon

~zmk-workspace/zmk# cd ..

~zmk-workspace$ devcontainer exec --workspace-folder ./zmk /bin/bash

#workspaces/zmk# ./zmk-cornedeon/build.sh <dongle|left|right|left_central>
