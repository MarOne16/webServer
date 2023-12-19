webserverخادم الويب 

## Table of Contents

- [Introduction](#introduction)
- [Features](#features)
- [Getting Started](#getting-started)
  - [Prerequisites](#prerequisites)
  - [Installation](#installation)
  - [Configuration](#configuration)
- [Usage](#usage)
- [Contributing](#contributing)
- [License](#license)
- [Acknowledgements](#acknowledgements)







## Introduction

in this project we build a Web Server like NGINX using C and C++98 (Not as efficient as nginx)









## Features

you can setup your configuration like nginx config file, the communication between server and client done by sockets.











## Getting Started
after cloning this project , you can find S_Book file if you need some information about using config file to setup your setting.










### Prerequisites

you need python bin and php bin for test cgi if you want run php or python in cgi,
if you need to test this project in linux you must to change some things in code and makfile, Because we built it in MacOs.











### Installation
first clone this repo after , enter make in terminal on main repo now webserv ready to use





```bash
# Example installation commands
git clone https://github.com/MarOne16/webServer.git webserv
cd webserv
make
./webserv
