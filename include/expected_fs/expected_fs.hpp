#ifndef EXPECTED_FS_EXPECTED_FS_HPP_
#define EXPECTED_FS_EXPECTED_FS_HPP_

#include <cstddef>
#include <cstdint>

#if defined(__has_include)
#if __has_include(<expected>)
#include <expected>
#endif
#else
#include <expected>
#endif

#include <filesystem>
#include <system_error>
#include <utility>

#if defined(__cpp_lib_format_path) && (__cpp_lib_format_path >= 202403L)
#define EXPECTED_FS_HAS_FORMAT_PATH 1
#else
#define EXPECTED_FS_HAS_FORMAT_PATH 0
#endif

#if !defined(__cpp_lib_expected) || (__cpp_lib_expected < 202202L)
#error "expected_fs requires C++23 <expected> support."
#else

namespace expected_fs {
using copy_options = std::filesystem::copy_options;
using directory_entry = std::filesystem::directory_entry;
using directory_iterator = std::filesystem::directory_iterator;
using directory_options = std::filesystem::directory_options;
using file_status = std::filesystem::file_status;
using file_time_type = std::filesystem::file_time_type;
using file_type = std::filesystem::file_type;
using filesystem_error = std::filesystem::filesystem_error;
using path = std::filesystem::path;
using perm_options = std::filesystem::perm_options;
using perms = std::filesystem::perms;
using recursive_directory_iterator =
    std::filesystem::recursive_directory_iterator;
using space_info = std::filesystem::space_info;

template <class T, class E = std::error_code>
using expected = std::expected<T, E>;

inline constexpr bool has_format_path = EXPECTED_FS_HAS_FORMAT_PATH != 0;

namespace detail {
template <class T>
[[nodiscard]] inline expected<T>
make_expected(T value, const std::error_code &error) {
  if (error) {
    return std::unexpected(error);
  }

  return expected<T>{std::move(value)};
}

[[nodiscard]] inline expected<void>
make_expected(const std::error_code &error) {
  if (error) {
    return std::unexpected(error);
  }

  return {};
}

[[nodiscard]] inline expected<file_status>
make_status_expected(file_status value, const std::error_code &error) {
  if (std::filesystem::status_known(value)) {
    return expected<file_status>{value};
  }

  return make_expected(value, error);
}

template <class Predicate>
[[nodiscard]] inline expected<bool>
query_status(const path &value, Predicate predicate) {
  std::error_code error;
  auto status =
      make_status_expected(std::filesystem::status(value, error), error);
  if (!status) {
    return std::unexpected(status.error());
  }

  return predicate(*status);
}

template <class Predicate>
[[nodiscard]] inline expected<bool>
query_symlink_status(const path &value, Predicate predicate) {
  std::error_code error;
  auto status = make_status_expected(
      std::filesystem::symlink_status(value, error), error);
  if (!status) {
    return std::unexpected(status.error());
  }

  return predicate(*status);
}

template <class Predicate>
[[nodiscard]] inline expected<bool>
query_status(const directory_entry &entry, Predicate predicate) {
  std::error_code error;
  auto status = make_status_expected(entry.status(error), error);
  if (!status) {
    return std::unexpected(status.error());
  }

  return predicate(*status);
}

template <class Predicate>
[[nodiscard]] inline expected<bool>
query_symlink_status(const directory_entry &entry, Predicate predicate) {
  std::error_code error;
  auto status = make_status_expected(entry.symlink_status(error), error);
  if (!status) {
    return std::unexpected(status.error());
  }

  return predicate(*status);
}
} // namespace detail

[[nodiscard]] inline path u8path(auto &&source) {
  return std::filesystem::u8path(std::forward<decltype(source)>(source));
}

template <class InputIterator>
[[nodiscard]] inline path u8path(InputIterator first, InputIterator last) {
  return std::filesystem::u8path(first, last);
}

inline void swap(path &left, path &right) noexcept {
  std::filesystem::swap(left, right);
}

[[nodiscard]] inline std::size_t hash_value(const path &value) noexcept {
  return std::filesystem::hash_value(value);
}

[[nodiscard]] inline expected<path> absolute(const path &value) {
  std::error_code error;
  auto result = std::filesystem::absolute(value, error);
  return detail::make_expected(std::move(result), error);
}

[[nodiscard]] inline expected<path> canonical(const path &value) {
  std::error_code error;
  auto result = std::filesystem::canonical(value, error);
  return detail::make_expected(std::move(result), error);
}

[[nodiscard]] inline expected<path> weakly_canonical(const path &value) {
  std::error_code error;
  auto result = std::filesystem::weakly_canonical(value, error);
  return detail::make_expected(std::move(result), error);
}

[[nodiscard]] inline expected<path> relative(const path &value) {
  std::error_code error;
  auto result = std::filesystem::relative(value, error);
  return detail::make_expected(std::move(result), error);
}

[[nodiscard]] inline expected<path>
relative(const path &value, const path &base) {
  std::error_code error;
  auto result = std::filesystem::relative(value, base, error);
  return detail::make_expected(std::move(result), error);
}

[[nodiscard]] inline expected<path> proximate(const path &value) {
  std::error_code error;
  auto result = std::filesystem::proximate(value, error);
  return detail::make_expected(std::move(result), error);
}

[[nodiscard]] inline expected<path>
proximate(const path &value, const path &base) {
  std::error_code error;
  auto result = std::filesystem::proximate(value, base, error);
  return detail::make_expected(std::move(result), error);
}

[[nodiscard]] inline expected<path> read_symlink(const path &value) {
  std::error_code error;
  auto result = std::filesystem::read_symlink(value, error);
  return detail::make_expected(std::move(result), error);
}

[[nodiscard]] inline expected<path> temp_directory_path() {
  std::error_code error;
  auto result = std::filesystem::temp_directory_path(error);
  return detail::make_expected(std::move(result), error);
}

[[nodiscard]] inline expected<path> current_path() {
  std::error_code error;
  auto result = std::filesystem::current_path(error);
  return detail::make_expected(std::move(result), error);
}

[[nodiscard]] inline expected<void> current_path(const path &value) {
  std::error_code error;
  std::filesystem::current_path(value, error);
  return detail::make_expected(error);
}

[[nodiscard]] inline bool status_known(file_status value) noexcept {
  return std::filesystem::status_known(value);
}

[[nodiscard]] inline expected<file_status> status(const path &value) {
  std::error_code error;
  auto result = std::filesystem::status(value, error);
  return detail::make_status_expected(result, error);
}

[[nodiscard]] inline expected<file_status>
status(const directory_entry &entry) {
  std::error_code error;
  auto result = entry.status(error);
  return detail::make_status_expected(result, error);
}

[[nodiscard]] inline expected<file_status> symlink_status(const path &value) {
  std::error_code error;
  auto result = std::filesystem::symlink_status(value, error);
  return detail::make_status_expected(result, error);
}

[[nodiscard]] inline expected<file_status>
symlink_status(const directory_entry &entry) {
  std::error_code error;
  auto result = entry.symlink_status(error);
  return detail::make_status_expected(result, error);
}

[[nodiscard]] inline bool exists(file_status value) noexcept {
  return std::filesystem::exists(value);
}

[[nodiscard]] inline expected<bool> exists(const path &value) {
  return detail::query_status(value, [](file_status status_value) {
    return std::filesystem::exists(status_value);
  });
}

[[nodiscard]] inline expected<bool> exists(const directory_entry &entry) {
  return detail::query_status(entry, [](file_status status_value) {
    return std::filesystem::exists(status_value);
  });
}

[[nodiscard]] inline expected<bool>
equivalent(const path &left, const path &right) {
  std::error_code error;
  auto value = std::filesystem::equivalent(left, right, error);
  return detail::make_expected(value, error);
}

[[nodiscard]] inline bool is_block_file(file_status value) noexcept {
  return std::filesystem::is_block_file(value);
}

[[nodiscard]] inline expected<bool> is_block_file(const path &value) {
  return detail::query_status(value, [](file_status status_value) {
    return std::filesystem::is_block_file(status_value);
  });
}

[[nodiscard]] inline expected<bool>
is_block_file(const directory_entry &entry) {
  return detail::query_status(entry, [](file_status status_value) {
    return std::filesystem::is_block_file(status_value);
  });
}

[[nodiscard]] inline bool is_character_file(file_status value) noexcept {
  return std::filesystem::is_character_file(value);
}

[[nodiscard]] inline expected<bool> is_character_file(const path &value) {
  return detail::query_status(value, [](file_status status_value) {
    return std::filesystem::is_character_file(status_value);
  });
}

[[nodiscard]] inline expected<bool>
is_character_file(const directory_entry &entry) {
  return detail::query_status(entry, [](file_status status_value) {
    return std::filesystem::is_character_file(status_value);
  });
}

[[nodiscard]] inline bool is_directory(file_status value) noexcept {
  return std::filesystem::is_directory(value);
}

[[nodiscard]] inline expected<bool> is_directory(const path &value) {
  return detail::query_status(value, [](file_status status_value) {
    return std::filesystem::is_directory(status_value);
  });
}

[[nodiscard]] inline expected<bool> is_directory(const directory_entry &entry) {
  return detail::query_status(entry, [](file_status status_value) {
    return std::filesystem::is_directory(status_value);
  });
}

[[nodiscard]] inline expected<bool> is_empty(const path &value) {
  std::error_code error;
  auto result = std::filesystem::is_empty(value, error);
  return detail::make_expected(result, error);
}

[[nodiscard]] inline expected<bool> is_empty(const directory_entry &entry) {
  return ::expected_fs::is_empty(entry.path());
}

[[nodiscard]] inline bool is_fifo(file_status value) noexcept {
  return std::filesystem::is_fifo(value);
}

[[nodiscard]] inline expected<bool> is_fifo(const path &value) {
  return detail::query_status(value, [](file_status status_value) {
    return std::filesystem::is_fifo(status_value);
  });
}

[[nodiscard]] inline expected<bool> is_fifo(const directory_entry &entry) {
  return detail::query_status(entry, [](file_status status_value) {
    return std::filesystem::is_fifo(status_value);
  });
}

[[nodiscard]] inline bool is_other(file_status value) noexcept {
  return std::filesystem::is_other(value);
}

[[nodiscard]] inline expected<bool> is_other(const path &value) {
  return detail::query_status(value, [](file_status status_value) {
    return std::filesystem::is_other(status_value);
  });
}

[[nodiscard]] inline expected<bool> is_other(const directory_entry &entry) {
  return detail::query_status(entry, [](file_status status_value) {
    return std::filesystem::is_other(status_value);
  });
}

[[nodiscard]] inline bool is_regular_file(file_status value) noexcept {
  return std::filesystem::is_regular_file(value);
}

[[nodiscard]] inline expected<bool> is_regular_file(const path &value) {
  return detail::query_status(value, [](file_status status_value) {
    return std::filesystem::is_regular_file(status_value);
  });
}

[[nodiscard]] inline expected<bool>
is_regular_file(const directory_entry &entry) {
  return detail::query_status(entry, [](file_status status_value) {
    return std::filesystem::is_regular_file(status_value);
  });
}

[[nodiscard]] inline bool is_socket(file_status value) noexcept {
  return std::filesystem::is_socket(value);
}

[[nodiscard]] inline expected<bool> is_socket(const path &value) {
  return detail::query_status(value, [](file_status status_value) {
    return std::filesystem::is_socket(status_value);
  });
}

[[nodiscard]] inline expected<bool> is_socket(const directory_entry &entry) {
  return detail::query_status(entry, [](file_status status_value) {
    return std::filesystem::is_socket(status_value);
  });
}

[[nodiscard]] inline bool is_symlink(file_status value) noexcept {
  return std::filesystem::is_symlink(value);
}

[[nodiscard]] inline expected<bool> is_symlink(const path &value) {
  return detail::query_symlink_status(value, [](file_status status_value) {
    return std::filesystem::is_symlink(status_value);
  });
}

[[nodiscard]] inline expected<bool> is_symlink(const directory_entry &entry) {
  return detail::query_symlink_status(entry, [](file_status status_value) {
    return std::filesystem::is_symlink(status_value);
  });
}

[[nodiscard]] inline expected<std::uintmax_t> file_size(const path &value) {
  std::error_code error;
  auto result = std::filesystem::file_size(value, error);
  return detail::make_expected(result, error);
}

[[nodiscard]] inline expected<std::uintmax_t>
file_size(const directory_entry &entry) {
  std::error_code error;
  auto result = entry.file_size(error);
  return detail::make_expected(result, error);
}

[[nodiscard]] inline expected<std::uintmax_t>
hard_link_count(const path &value) {
  std::error_code error;
  auto result = std::filesystem::hard_link_count(value, error);
  return detail::make_expected(result, error);
}

[[nodiscard]] inline expected<std::uintmax_t>
hard_link_count(const directory_entry &entry) {
  std::error_code error;
  auto result = entry.hard_link_count(error);
  return detail::make_expected(result, error);
}

[[nodiscard]] inline expected<file_time_type>
last_write_time(const path &value) {
  std::error_code error;
  auto result = std::filesystem::last_write_time(value, error);
  return detail::make_expected(result, error);
}

[[nodiscard]] inline expected<file_time_type>
last_write_time(const directory_entry &entry) {
  std::error_code error;
  auto result = entry.last_write_time(error);
  return detail::make_expected(result, error);
}

[[nodiscard]] inline expected<void>
last_write_time(const path &value, file_time_type new_time) {
  std::error_code error;
  std::filesystem::last_write_time(value, new_time, error);
  return detail::make_expected(error);
}

[[nodiscard]] inline expected<space_info> space(const path &value) {
  std::error_code error;
  auto result = std::filesystem::space(value, error);
  return detail::make_expected(result, error);
}

[[nodiscard]] inline expected<void>
copy(const path &from, const path &to,
     copy_options options = copy_options::none) {
  std::error_code error;
  std::filesystem::copy(from, to, options, error);
  return detail::make_expected(error);
}

[[nodiscard]] inline expected<bool>
copy_file(const path &from, const path &to,
          copy_options options = copy_options::none) {
  std::error_code error;
  auto result = std::filesystem::copy_file(from, to, options, error);
  return detail::make_expected(result, error);
}

[[nodiscard]] inline expected<void>
copy_symlink(const path &from, const path &to) {
  std::error_code error;
  std::filesystem::copy_symlink(from, to, error);
  return detail::make_expected(error);
}

[[nodiscard]] inline expected<bool> create_directory(const path &value) {
  std::error_code error;
  auto result = std::filesystem::create_directory(value, error);
  return detail::make_expected(result, error);
}

[[nodiscard]] inline expected<bool>
create_directory(const path &value, const path &attributes) {
  std::error_code error;
  auto result = std::filesystem::create_directory(value, attributes, error);
  return detail::make_expected(result, error);
}

[[nodiscard]] inline expected<bool> create_directories(const path &value) {
  std::error_code error;
  auto result = std::filesystem::create_directories(value, error);
  return detail::make_expected(result, error);
}

[[nodiscard]] inline expected<void>
create_hard_link(const path &target, const path &link) {
  std::error_code error;
  std::filesystem::create_hard_link(target, link, error);
  return detail::make_expected(error);
}

[[nodiscard]] inline expected<void>
create_symlink(const path &target, const path &link) {
  std::error_code error;
  std::filesystem::create_symlink(target, link, error);
  return detail::make_expected(error);
}

[[nodiscard]] inline expected<void>
create_directory_symlink(const path &target, const path &link) {
  std::error_code error;
  std::filesystem::create_directory_symlink(target, link, error);
  return detail::make_expected(error);
}

[[nodiscard]] inline expected<void>
permissions(const path &value, perms new_permissions,
            perm_options options = perm_options::replace) {
  std::error_code error;
  std::filesystem::permissions(value, new_permissions, options, error);
  return detail::make_expected(error);
}

[[nodiscard]] inline expected<bool> remove(const path &value) {
  std::error_code error;
  auto result = std::filesystem::remove(value, error);
  return detail::make_expected(result, error);
}

[[nodiscard]] inline expected<std::uintmax_t> remove_all(const path &value) {
  std::error_code error;
  auto result = std::filesystem::remove_all(value, error);
  return detail::make_expected(result, error);
}

[[nodiscard]] inline expected<void> rename(const path &from, const path &to) {
  std::error_code error;
  std::filesystem::rename(from, to, error);
  return detail::make_expected(error);
}

[[nodiscard]] inline expected<void>
resize_file(const path &value, std::uintmax_t size) {
  std::error_code error;
  std::filesystem::resize_file(value, size, error);
  return detail::make_expected(error);
}

[[nodiscard]] inline expected<directory_entry>
make_directory_entry(const path &value) {
  std::error_code error;
  directory_entry entry(value, error);
  return detail::make_expected(std::move(entry), error);
}

[[nodiscard]] inline expected<void>
assign(directory_entry &entry, const path &value) {
  std::error_code error;
  entry.assign(value, error);
  return detail::make_expected(error);
}

[[nodiscard]] inline expected<void>
replace_filename(directory_entry &entry, const path &value) {
  std::error_code error;
  entry.replace_filename(value, error);
  return detail::make_expected(error);
}

[[nodiscard]] inline expected<void> refresh(directory_entry &entry) {
  std::error_code error;
  entry.refresh(error);
  return detail::make_expected(error);
}

[[nodiscard]] inline expected<directory_iterator>
make_directory_iterator(const path &value,
                        directory_options options = directory_options::none) {
  std::error_code error;
  directory_iterator iterator(value, options, error);
  return detail::make_expected(std::move(iterator), error);
}

[[nodiscard]] inline expected<void> increment(directory_iterator &iterator) {
  std::error_code error;
  iterator.increment(error);
  return detail::make_expected(error);
}

[[nodiscard]] inline directory_iterator
begin(directory_iterator iterator) noexcept {
  return std::filesystem::begin(std::move(iterator));
}

[[nodiscard]] inline directory_iterator
end(directory_iterator iterator) noexcept {
  return std::filesystem::end(std::move(iterator));
}

[[nodiscard]] inline expected<recursive_directory_iterator>
make_recursive_directory_iterator(
    const path &value, directory_options options = directory_options::none) {
  std::error_code error;
  recursive_directory_iterator iterator(value, options, error);
  return detail::make_expected(std::move(iterator), error);
}

[[nodiscard]] inline directory_options
options(const recursive_directory_iterator &iterator) noexcept {
  return iterator.options();
}

[[nodiscard]] inline int
depth(const recursive_directory_iterator &iterator) noexcept {
  return iterator.depth();
}

[[nodiscard]] inline bool
recursion_pending(const recursive_directory_iterator &iterator) noexcept {
  return iterator.recursion_pending();
}

inline void
disable_recursion_pending(recursive_directory_iterator &iterator) noexcept {
  iterator.disable_recursion_pending();
}

[[nodiscard]] inline expected<void>
increment(recursive_directory_iterator &iterator) {
  std::error_code error;
  iterator.increment(error);
  return detail::make_expected(error);
}

[[nodiscard]] inline expected<void>
pop(recursive_directory_iterator &iterator) {
  std::error_code error;
  iterator.pop(error);
  return detail::make_expected(error);
}

[[nodiscard]] inline recursive_directory_iterator
begin(recursive_directory_iterator iterator) noexcept {
  return std::filesystem::begin(std::move(iterator));
}

[[nodiscard]] inline recursive_directory_iterator
end(recursive_directory_iterator iterator) noexcept {
  return std::filesystem::end(std::move(iterator));
}
} // namespace expected_fs

#endif

#endif // EXPECTED_FS_EXPECTED_FS_HPP_
