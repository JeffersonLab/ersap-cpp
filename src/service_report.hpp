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

#ifndef ERSAP_SERVICE_REPORT_HPP
#define ERSAP_SERVICE_REPORT_HPP

#include "constants.hpp"

#include <atomic>
#include <string>

namespace ersap {

struct ServiceParameters
{
    std::string engine_name;
    std::string engine_lib;
    std::string initial_state;
    std::string description;
    int pool_size;
};


class ServiceReport
{
public:
    ServiceReport(const std::string& name,
                  const ServiceParameters& params,
                  const std::string& author,
                  const std::string& version,
                  const std::string& description);

public:
    std::string name() const { return name_; };

    std::string engine() const { return engine_; };

    std::string library() const { return library_; };

    std::string author() const { return author_; };

    std::string lang() const { return constants::cpp_lang; };

    int pool_size() const { return pool_size_; };

    std::string description() const { return description_; };

    std::string version() const { return version_; };

    std::string start_time() const { return start_time_; };

public:
    long n_requests() const { return n_requests_.load(); };

    long n_failures() const { return n_failures_.load(); };

    long shm_reads() const { return shm_reads_.load(); };

    long shm_writes() const { return shm_writes_.load(); };

    long bytes_recv() const { return bytes_recv_.load(); };

    long bytes_sent() const { return bytes_sent_.load(); };

    long exec_time() const { return exec_time_.load(); };

public:
    void add_n_requests() { n_requests_.fetch_add(1); };

    void add_n_failures() { n_failures_.fetch_add(1); };

    void add_shm_reads() { shm_reads_.fetch_add(1); };

    void add_shm_writes() { shm_writes_.fetch_add(1); };

    void add_bytes_recv(std::int64_t bytes) { bytes_recv_.fetch_add(bytes); };

    void add_bytes_sent(std::int64_t bytes) { bytes_sent_.fetch_add(bytes); };

    void add_exec_time(std::int64_t duration) { exec_time_.fetch_add(duration); };

private:
    std::string name_;
    std::string engine_;
    std::string library_;

    int pool_size_;
    std::string author_;
    std::string version_;
    std::string description_;
    std::string start_time_;

    std::atomic<std::int64_t> n_requests_{0};
    std::atomic<std::int64_t> n_failures_{0};
    std::atomic<std::int64_t> shm_reads_{0};
    std::atomic<std::int64_t> shm_writes_{0};
    std::atomic<std::int64_t> bytes_recv_{0};
    std::atomic<std::int64_t> bytes_sent_{0};
    std::atomic<std::int64_t> exec_time_{0};
};

} // end namespace ersap

#endif // end of include guard: ERSAP_SERVICE_REPORT_HPP
