/**
 * Copyright (c) 2021 OceanBase
 * OceanBase CE is licensed under Mulan PubL v2.
 * You can use this software according to the terms and conditions of the Mulan PubL v2.
 * You may obtain a copy of Mulan PubL v2 at:
 *          http://license.coscl.org.cn/MulanPubL-2.0
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PubL v2 for more details.
 */

#define USING_LOG_PREFIX SERVER
#include "observer/ob_check_params.h"

namespace oceanbase
{
using namespace common;
namespace observer
{
int CheckAllParams::check_all_params(bool strict_check = true)
{
  int ret = OB_SUCCESS;
  if (OB_SUCC(ret) && OB_FAIL(check_max_map_count(strict_check))) {
    LOG_DBA_ERROR(OB_ERR_UNEXPECTED, "msg", "check max_map_count failed");
  }

  if (OB_SUCC(ret) && OB_FAIL(check_vm_min_free_kbytes(strict_check))) {
    LOG_DBA_ERROR(OB_ERR_UNEXPECTED, "msg", "check vm_min_free_kbytes failed");
  }

  if (OB_SUCC(ret) && OB_FAIL(check_vm_overcommit_memory(strict_check))) {
    LOG_DBA_ERROR(OB_ERR_UNEXPECTED, "msg", "check vm_overcommit_memory failed");
  }

  if (OB_SUCC(ret) && OB_FAIL(check_fs_file_max(strict_check))) {
    LOG_DBA_ERROR(OB_ERR_UNEXPECTED, "msg", "check fs_file_max failed");
  }

  if (OB_SUCC(ret) && OB_FAIL(check_ulimit_open_files(strict_check))) {
    LOG_DBA_ERROR(OB_ERR_UNEXPECTED, "msg", "check ulimit_open_files failed");
  }

  if (OB_SUCC(ret) && OB_FAIL(check_ulimit_process_limit(strict_check))) {
    LOG_DBA_ERROR(OB_ERR_UNEXPECTED, "msg", "check ulimit_process_limit failed");
  }

  if (OB_SUCC(ret) && OB_FAIL(check_ulimit_core_file_size(strict_check))) {
    LOG_DBA_ERROR(OB_ERR_UNEXPECTED, "msg", "check ulimit_core_file_size failed");
  }

  if (OB_SUCC(ret) && OB_FAIL(check_ulimit_stack_size(strict_check))) {
    LOG_DBA_ERROR(OB_ERR_UNEXPECTED, "msg", "check ulimit_stack_size failed");
  }
  return ret;
}

int CheckAllParams::read_one_int(const char* file_name, int64_t &value) {
  int ret = OB_SUCCESS;
  FILE* fp = fopen(file_name, "r");
  if (fp != nullptr) {
    if (1 != fscanf(fp, "%ld", &value)) {
      ret = OB_IO_ERROR;
      LOG_ERROR("Failed to read integer from file", K(ret));
    }
    fclose(fp);
  } else {
    ret = OB_ERROR;
    LOG_WARN("File does not exist");
  }
  return ret;
}


bool CheckAllParams::is_path_valid(const char *file_name)
{
  return (access(file_name, F_OK) != -1);
}

int CheckAllParams::check_max_map_count(bool strict_check)
{
  int ret = OB_SUCCESS;
  const char *file_path = "/proc/sys/vm/max_map_count";
  if (is_path_valid(file_path)) {
    int64_t max_map_count = 0;
    if (OB_FAIL(read_one_int(file_path, max_map_count))) {
      LOG_DBA_ERROR(OB_ERR_UNEXPECTED, "msg", "read file failed", K(file_path));
    } else if (max_map_count >= 327600 && max_map_count <= 1000000) {
      LOG_INFO("vm.max_map_count is within the range", K(max_map_count));
    } else {
      if (strict_check) {
        ret = OB_ERR_UNEXPECTED;
        LOG_DBA_ERROR(OB_ERR_UNEXPECTED, "msg", "vm.max_map_count is not within the range [327600, 1000000]", K(max_map_count));
      } else {
        LOG_WARN("vm.max_map_count is not within the range [327600, 1000000]", K(max_map_count));
      }
    }
  } else {
    LOG_WARN("file path does not exist", K(file_path));
  }
  return ret;
}

int CheckAllParams::check_vm_min_free_kbytes(bool strict_check) {
  int ret = OB_SUCCESS;
  const char* file_path = "/proc/sys/vm/min_free_kbytes";
  if (is_path_valid(file_path)) {
    int64_t vm_min_free_kbytes = 0;
    if (OB_FAIL(read_one_int(file_path, vm_min_free_kbytes))) {
      LOG_DBA_ERROR(OB_ERR_UNEXPECTED, "msg", "read file failed", K(file_path));
    } else if (vm_min_free_kbytes >= 32768 && vm_min_free_kbytes <= 2097152) {
      LOG_INFO("vm.min_free_kbytes is within the range", K(vm_min_free_kbytes));
    } else {
      if (strict_check) {
        ret = OB_ERR_UNEXPECTED;
        LOG_DBA_ERROR(OB_ERR_UNEXPECTED,"msg", "vm.min_free_kbytes is not within the allowed range [32768, 2097152]", K(vm_min_free_kbytes));
      } else {
        LOG_WARN("vm.min_free_kbytes is not within the allowed range [32768, 2097152]", K(vm_min_free_kbytes));
      }
    }
  } else {
    LOG_WARN("file path does not exist", K(file_path));
  }
  return ret;
}

int CheckAllParams::check_vm_overcommit_memory(bool strict_check) {
  int ret = OB_SUCCESS;
  const char* file_path = "/proc/sys/vm/overcommit_memory";
  if (is_path_valid(file_path)) {
    int64_t vm_overcommit_memory = 0;
    if (OB_FAIL(read_one_int(file_path, vm_overcommit_memory))) {
      LOG_DBA_ERROR(OB_ERR_UNEXPECTED, "msg", "read file failed", K(file_path));
    } else if (vm_overcommit_memory == 0) {
      LOG_INFO("vm.overcommit_memory is equal to 0", K(vm_overcommit_memory));
    } else {
      if (strict_check) {
        ret = OB_ERR_UNEXPECTED;
        LOG_DBA_ERROR(OB_ERR_UNEXPECTED,"msg", "vm.overcommit_memory is not the value:0", K(vm_overcommit_memory));
      } else {
        LOG_WARN("vm.overcommit_memory is not within the allowed value:0", K(vm_overcommit_memory));
      }
    }
  } else {
    LOG_WARN("file path does not exist", K(file_path));
  }
  return ret;
}

int CheckAllParams::check_fs_file_max(bool strict_check) {
  int ret = OB_SUCCESS;
  const char* file_path = "/proc/sys/fs/file-max";
  if (is_path_valid(file_path)) {
    int64_t fs_file_max = 0;
    if (OB_FAIL(read_one_int(file_path, fs_file_max))) {
      LOG_DBA_ERROR(OB_ERR_UNEXPECTED, "msg", "read file failed", K(file_path));
    } else if (fs_file_max >= 6573688) {
      LOG_INFO("fs.file-max is greater than or equal to 6573688", K(fs_file_max));
    } else {
      if (strict_check) {
        ret = OB_ERR_UNEXPECTED;
        LOG_DBA_ERROR(OB_ERR_UNEXPECTED,"msg", "fs.file-max is less than 6573688", K(fs_file_max));
      } else {
        LOG_WARN("fs.file-max is less than 6573688", K(fs_file_max));
      }
    }
  } else {
    LOG_WARN("file path does not exist", K(file_path));
  }
  return ret;
}

int CheckAllParams::check_ulimit_open_files(bool strict_check)
{
  int ret = OB_SUCCESS;
  // Check open files limit
  struct rlimit rlim;
  if (getrlimit(RLIMIT_NOFILE, &rlim) == 0) {
    if (rlim.rlim_cur >= 655300 && rlim.rlim_max >= 655300) {
      LOG_INFO("open files limit is greater than or equal to 655300", K(rlim.rlim_cur), K(rlim.rlim_max));
    } else {
      if (strict_check) {
        ret = OB_ERR_UNEXPECTED;
        LOG_DBA_ERROR(OB_ERR_UNEXPECTED, "msg", "open files limit's soft nofile or hard nofile is less than 655300", K(rlim.rlim_cur), K(rlim.rlim_max));
      } else {
        LOG_WARN("open files limit's soft nofile or hard nofile is less than 655300", K(rlim.rlim_cur), K(rlim.rlim_max));
      }
    }
  } else {
      ret = OB_ERR_UNEXPECTED;
      LOG_DBA_ERROR(OB_ERR_UNEXPECTED, "msg", "read ulimit_open_file file failed");
  }
  return ret;
}

int CheckAllParams::check_ulimit_process_limit(bool strict_check)
{
  int ret = OB_SUCCESS;
  struct rlimit rlim;
  // Check process limit
  if (getrlimit(RLIMIT_NPROC, &rlim) == 0) {
    if (rlim.rlim_cur >= 655300 && rlim.rlim_max >= 655300) {
      LOG_INFO("process limit is greater than or equal to 655300", K(rlim.rlim_cur), K(rlim.rlim_max));
    } else {
      if (strict_check) {
        ret = OB_ERR_UNEXPECTED;
        LOG_DBA_ERROR(OB_ERR_UNEXPECTED, "msg", "process limit's soft nofile or hard nofile is less than 655300", K(rlim.rlim_cur), K(rlim.rlim_max));
      } else {
        LOG_WARN("process limit's soft nofile or hard nofile is less than 655300", K(rlim.rlim_cur), K(rlim.rlim_max));
      }
    }
  } else {
    ret = OB_ERR_UNEXPECTED;
    LOG_DBA_ERROR(OB_ERR_UNEXPECTED, "msg", "read ulimit_process_limit file failed");
  }
  return ret;
}
int CheckAllParams::check_ulimit_core_file_size(bool strict_check)
{
  int ret = OB_SUCCESS;
  struct rlimit rlim;
  // Check core file size limit
  if (getrlimit(RLIMIT_CORE, &rlim) == 0) {
    if (rlim.rlim_cur == RLIM_INFINITY && rlim.rlim_max == RLIM_INFINITY) {
      LOG_INFO("core file size limit is unlimited");
    } else {
      //Violations of the recommended range will only trigger a warning, regardless of strict or non-strict checking.
      LOG_WARN("core file size limit's soft nofile or hard nofile is limited", K(rlim.rlim_cur), K(rlim.rlim_max));
    }
  } else {
    ret = OB_ERR_UNEXPECTED;
    LOG_DBA_ERROR(OB_ERR_UNEXPECTED, "msg", "read ulimit_core_file_size file failed");
  }
  return ret;
}

int CheckAllParams::check_ulimit_stack_size(bool strict_check)
{
  int ret = OB_SUCCESS;
  struct rlimit rlim;
  // Check stack size limit
  if (getrlimit(RLIMIT_STACK, &rlim) == 0) {
    if (rlim.rlim_cur >= (1 << 20) && rlim.rlim_max >= (1 << 20)) {
      LOG_INFO("stack size limit is larger than 1M", K(rlim.rlim_cur), K(rlim.rlim_max));
    } else {
      if (strict_check) {
        ret = OB_ERR_UNEXPECTED;
        LOG_DBA_ERROR(OB_ERR_UNEXPECTED, "msg", "stack size limit's soft nofile or hard nofile is smaller than 1M", K(rlim.rlim_cur), K(rlim.rlim_max));
      } else {
        LOG_WARN("stack size limit's soft nofile or hard nofile is smaller than 1M", K(rlim.rlim_cur), K(rlim.rlim_max));
      }
    }
  } else {
    ret = OB_ERR_UNEXPECTED;
    LOG_DBA_ERROR(OB_ERR_UNEXPECTED, "msg", "read ulimit_stack_size file failed");
  }
  return ret;
}


int check_os_params(bool strict_check_params = false)
{
  int ret = OB_SUCCESS;
  if (OB_FAIL(CheckAllParams::check_all_params(strict_check_params))) {
    LOG_DBA_ERROR(OB_ERR_UNEXPECTED, "msg", "check os params failed");
  }
  return ret;
}
}  // namespace observer
}  // namespace oceanbase
