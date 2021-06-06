![C3 Bridge Interface](https://github.com/ulsu-tech/c3bridge-server/raw/master/artwork/logo.png)
# C3 Bridge Interface Server
The C3 Bridge Interface Server is a lightweight network application that allows remote clients to execute requests to the KUKA Cross 3 subsystem and return responses. The application provides advanced functionality and high performance.

## Features
* Well-documented [application protocol](https://github.com/ulsu-tech/c3bridge-server/raw/master/documents/C3%20Bridge%20Interface%20Protocol%201.3.pdf). It is fully compatible with KukavarProxy protocol, which means that existing client solutions (RoboDK, ROS) can be used.
* Executes more functions of the KUKA Cross 3 subsystem (in the future we hope to implement all available functions).
* Controls the execution of a KRL program.
* High performance. The software is written in C++ and can be compiled using both modern development tools and development tools of the past years (like Visual C++ 6.0).
* It does not require additional software libraries to be installed in the control system.
* It starts minimized to tray and does not take up any space on the screen.
* There are no limits on the number of simultaneous network connections and idle time.
* It provides the Discovery Protocol for easy identification of the control system in the network.

## Changelog
See the [document](CHANGELOG.md) for more information.

## License
Copyright (c) 2020-2021 Dmitry Lavygin (vdm.inbox@gmail.com)

S.P. Kapitsa Research Institute of Technology of Ulyanovsk State University.

All rights reserved.


Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

## Donate
If you found this software useful, you can thank the author with donations via

[![PayPal](https://github.com/ulsu-tech/c3bridge-server/raw/master/artwork/donate.png)](https://www.paypal.com/cgi-bin/webscr?cmd=_donations&business=vdm.inbox@gmail.com&lc=EN&item_name=C3BridgeInterface&no_note=0&currency_code=USD&bn=PP-DonationsBF:btn_donateCC_LG.gif:NonHosted) [![Yandex.Money](https://github.com/ulsu-tech/c3bridge-server/raw/master/artwork/donate_yandex.png)](https://money.yandex.ru/to/4100111653323774)
