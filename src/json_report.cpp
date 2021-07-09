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

#include "json_report.hpp"

#include "dpe_report.hpp"
#include "container_report.hpp"
#include "json_utils.hpp"
#include "service_report.hpp"
#include "utils.hpp"

#include <string>


namespace ersap {

std::string JsonReport::generate(const DpeReport& report) const
{
    using namespace util;

    Buffer buffer;
    Writer writer{buffer};

    auto snapshot_time = util::get_current_time();

    writer.StartObject();

    writer.Key(constants::runtime_key.c_str());
    writer.StartObject();
    put(writer, "name", report.name());
    put(writer, "snapshot_time", snapshot_time);
    put(writer, "cpu_usage", report.cpu_usage());
    put(writer, "memory_usage", report.memory_usage());
    put(writer, "load", report.load());
    writer.Key("containers");
    writer.StartArray();
    for (const auto& cr : report.containers()) {
        auto cont_requests = 0L;
        writer.StartObject();
        put(writer, "name", cr->name());
        put(writer, "snapshot_time", snapshot_time);
        writer.Key("services");
        writer.StartArray();
        for (const auto& sr : cr->services()) {
            auto service_requests = sr->n_requests();
            cont_requests += service_requests;
            writer.StartObject();
            put(writer, "name", sr->name());
            put(writer, "snapshot_time", snapshot_time);
            put(writer, "n_requests", service_requests);
            put(writer, "n_failures", sr->n_failures());
            put(writer, "shm_reads", sr->shm_reads());
            put(writer, "shm_writes", sr->shm_writes());
            put(writer, "bytes_recv", sr->bytes_recv());
            put(writer, "bytes_sent", sr->bytes_sent());
            put(writer, "exec_time", sr->exec_time());
            writer.EndObject();
        }
        writer.EndArray();
        put(writer, "n_requests", cont_requests);
        writer.EndObject();
    }
    writer.EndArray();
    writer.EndObject();

    writer.Key(constants::registration_key.c_str());
    writer.StartObject();
    put(writer, "name", report.name());
    put(writer, "language", report.lang());
    put(writer, "ersap_home", report.ersap_home());
    put(writer, "n_cores", report.core_count());
    put(writer, "memory_size", report.memory_size());
    put(writer, "start_time", report.start_time());
    writer.Key("containers");
    writer.StartArray();
    for (const auto& cr : report.containers()) {
        writer.StartObject();
        put(writer, "name", cr->name());
        put(writer, "language", cr->lang());
        put(writer, "author", cr->author());
        put(writer, "start_time", cr->start_time());
        writer.Key("services");
        writer.StartArray();
        for (const auto& sr : cr->services()) {
            writer.StartObject();
            put(writer, "name", sr->name());
            put(writer, "class_name", sr->library());
            put(writer, "author", sr->author());
            put(writer, "version", sr->version());
            put(writer, "description", sr->description());
            put(writer, "language", sr->lang());
            put(writer, "pool_size", sr->pool_size());
            put(writer, "start_time", sr->start_time());
            writer.EndObject();
        }
        writer.EndArray();
        writer.EndObject();
    }
    writer.EndArray();
    writer.EndObject();

    writer.EndObject();

    return buffer.GetString();
}

} // end namespace ersap
