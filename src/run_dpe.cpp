/*
 * Copyright (c) 2016.  Jefferson Lab (JLab). All rights reserved. Permission
 * to use, copy, modify, and distribute  this software and its documentation for
 * educational, research, and not-for-profit purposes, without fee and without a
 * signed licensing agreement.
 *
 * IN NO EVENT SHALL JLAB BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL
 * INCIDENTAL, OR CONSEQUENTIAL DAMAGES, INCLUDING LOST PROFITS, ARISING
 * OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF JLAB HAS
 * BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * JLAB SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE. THE ERSAP SOFTWARE AND ACCOMPANYING DOCUMENTATION, IF ANY,
 * PROVIDED HEREUNDER IS PROVIDED "AS IS". JLAB HAS NO OBLIGATION TO PROVIDE
 * MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
 *
 * This software was developed under the United States Government license.
 * For more information contact author at gurjyan@jlab.org
 * Department of Experimental Nuclear Physics, Jefferson Lab.
 */

#include "dpe.hpp"
#include "dpe_options.hpp"

#include <xmsg/context.h>

#include <csignal>
#include <iostream>
#include <string>

#include <unistd.h>


volatile sig_atomic_t quit;

void signal_handler(int sig)
{
    std::cout << std::endl;
    signal(sig, signal_handler);
    quit = 1;
}


int main(int argc, char** argv)
{
    auto options = ersap::option::DpeOptionsParser{};
    if (!options.parse(argc, argv)) {
        return EXIT_FAILURE;
    }
    if (options.has_help()) {
        options.print_help();
        return EXIT_SUCCESS;
    }

    auto ctx = xmsg::Context::instance();
    ctx->set_io_threads(options.io_threads());
    ctx->set_max_sockets(options.max_sockets());

    ersap::Dpe dpe{false,
                   options.local_address(),
                   options.frontend_address(),
                   options.config()};
    dpe.start();

    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    while (quit == 0) {
        sleep(1);
    }

    return (EXIT_SUCCESS);
};
