#include "expected_fs/expected_fs.hpp"

#include <catch2/catch_test_macros.hpp>

#include <chrono>
#include <expected>
#include <filesystem>
#if EXPECTED_FS_HAS_FORMAT_PATH
#include <format>
#endif
#include <fstream>
#include <set>
#include <stdexcept>
#include <string>
#include <system_error>
#include <type_traits>
#include <utility>

namespace
{
  namespace fs = std::filesystem;

  class temporary_directory
  {
  public:
    temporary_directory()
    {
      const auto temp = fs::temp_directory_path();
      const auto seed = std::chrono::steady_clock::now().time_since_epoch().count();

      for(int attempt = 0; attempt < 100; ++attempt)
      {
        const auto candidate =
          temp / ("expected_fs_test_" + std::to_string(seed) + "_" + std::to_string(attempt));

        std::error_code error;
        if(fs::create_directory(candidate, error))
        {
          path_ = candidate;
          return;
        }
      }

      throw std::runtime_error("failed to create temporary test directory");
    }

    temporary_directory(const temporary_directory&) = delete;
    temporary_directory& operator=(const temporary_directory&) = delete;

    ~temporary_directory()
    {
      std::error_code error;
      fs::remove_all(path_, error);
    }

    [[nodiscard]] const fs::path& path() const noexcept { return path_; }

  private:
    fs::path path_;
  };

  class current_path_guard
  {
  public:
    explicit current_path_guard(fs::path original_path)
      : original_path_(std::move(original_path))
    {
    }

    current_path_guard(const current_path_guard&) = delete;
    current_path_guard& operator=(const current_path_guard&) = delete;

    ~current_path_guard()
    {
      std::error_code error;
      fs::current_path(original_path_, error);
    }

  private:
    fs::path original_path_;
  };

  void write_text(const fs::path& path, const std::string& contents)
  {
    std::ofstream stream(path, std::ios::binary);
    if(!stream)
    {
      throw std::runtime_error("failed to open test file for writing");
    }

    stream << contents;
  }
}  // namespace

TEST_CASE("expected_fs exposes std::expected compatible types", "[api]")
{
  static_assert(std::is_same_v<expected_fs::path, std::filesystem::path>);
  static_assert(std::is_same_v<expected_fs::directory_entry, std::filesystem::directory_entry>);
  static_assert(std::is_same_v<expected_fs::directory_iterator, std::filesystem::directory_iterator>);
  static_assert(
    std::is_same_v<expected_fs::recursive_directory_iterator, std::filesystem::recursive_directory_iterator>);
  static_assert(std::is_same_v<expected_fs::file_status, std::filesystem::file_status>);
  static_assert(std::is_same_v<expected_fs::space_info, std::filesystem::space_info>);
  static_assert(std::is_same_v<expected_fs::expected<int>, std::expected<int, std::error_code>>);
  static_assert(std::is_same_v<expected_fs::expected<int, std::errc>, std::expected<int, std::errc>>);
#if EXPECTED_FS_HAS_FORMAT_PATH
  static_assert(expected_fs::has_format_path);
#else
  static_assert(!expected_fs::has_format_path);
#endif
  static_assert(
    std::is_same_v<decltype(expected_fs::exists(std::declval<const fs::path&>())), expected_fs::expected<bool>>);
  static_assert(std::is_same_v<decltype(expected_fs::exists(std::declval<fs::file_status>())), bool>);
  static_assert(
    std::is_same_v<decltype(expected_fs::exists(std::declval<const fs::directory_entry&>())), expected_fs::expected<bool>>);
  static_assert(
    std::is_same_v<decltype(expected_fs::current_path(std::declval<const fs::path&>())), expected_fs::expected<void>>);
  static_assert(std::is_same_v<decltype(expected_fs::status_known(std::declval<fs::file_status>())), bool>);
  static_assert(
    std::is_same_v<decltype(expected_fs::is_block_file(std::declval<const fs::path&>())), expected_fs::expected<bool>>);
  static_assert(std::is_same_v<decltype(expected_fs::is_block_file(std::declval<fs::file_status>())), bool>);
  static_assert(
    std::is_same_v<decltype(expected_fs::is_character_file(std::declval<const fs::directory_entry&>())),
                   expected_fs::expected<bool>>);
  static_assert(
    std::is_same_v<decltype(expected_fs::is_fifo(std::declval<const fs::path&>())), expected_fs::expected<bool>>);
  static_assert(
    std::is_same_v<decltype(expected_fs::is_other(std::declval<const fs::directory_entry&>())),
                   expected_fs::expected<bool>>);
  static_assert(
    std::is_same_v<decltype(expected_fs::is_socket(std::declval<const fs::path&>())), expected_fs::expected<bool>>);
  static_assert(
    std::is_same_v<decltype(expected_fs::status(std::declval<const fs::directory_entry&>())),
                   expected_fs::expected<fs::file_status>>);
  static_assert(
    std::is_same_v<decltype(expected_fs::symlink_status(std::declval<const fs::directory_entry&>())),
                   expected_fs::expected<fs::file_status>>);
  static_assert(
    std::is_same_v<decltype(expected_fs::file_size(std::declval<const fs::directory_entry&>())),
                   expected_fs::expected<std::uintmax_t>>);
  static_assert(
    std::is_same_v<decltype(expected_fs::last_write_time(std::declval<const fs::directory_entry&>())),
                   expected_fs::expected<fs::file_time_type>>);
  static_assert(std::is_same_v<decltype(expected_fs::copy_symlink(std::declval<const fs::path&>(),
                                                                  std::declval<const fs::path&>())),
                               expected_fs::expected<void>>);
  static_assert(std::is_same_v<decltype(expected_fs::create_directory_symlink(std::declval<const fs::path&>(),
                                                                              std::declval<const fs::path&>())),
                               expected_fs::expected<void>>);
  static_assert(std::is_same_v<decltype(expected_fs::create_symlink(std::declval<const fs::path&>(),
                                                                    std::declval<const fs::path&>())),
                               expected_fs::expected<void>>);
  static_assert(std::is_same_v<decltype(expected_fs::create_hard_link(std::declval<const fs::path&>(),
                                                                      std::declval<const fs::path&>())),
                               expected_fs::expected<void>>);
  static_assert(
    std::is_same_v<decltype(expected_fs::make_directory_entry(std::declval<const fs::path&>())),
                   expected_fs::expected<fs::directory_entry>>);
  static_assert(std::is_same_v<decltype(expected_fs::assign(std::declval<fs::directory_entry&>(),
                                                            std::declval<const fs::path&>())),
                               expected_fs::expected<void>>);
  static_assert(std::is_same_v<decltype(expected_fs::replace_filename(std::declval<fs::directory_entry&>(),
                                                                      std::declval<const fs::path&>())),
                               expected_fs::expected<void>>);
  static_assert(
    std::is_same_v<decltype(expected_fs::refresh(std::declval<fs::directory_entry&>())), expected_fs::expected<void>>);
  static_assert(
    std::is_same_v<decltype(expected_fs::make_directory_iterator(std::declval<const fs::path&>())),
                   expected_fs::expected<fs::directory_iterator>>);
  static_assert(
    std::is_same_v<decltype(expected_fs::increment(std::declval<fs::directory_iterator&>())),
                   expected_fs::expected<void>>);
  static_assert(
    std::is_same_v<decltype(expected_fs::make_recursive_directory_iterator(std::declval<const fs::path&>())),
                   expected_fs::expected<fs::recursive_directory_iterator>>);
  static_assert(
    std::is_same_v<decltype(expected_fs::increment(std::declval<fs::recursive_directory_iterator&>())),
                   expected_fs::expected<void>>);
  static_assert(
    std::is_same_v<decltype(expected_fs::pop(std::declval<fs::recursive_directory_iterator&>())),
                   expected_fs::expected<void>>);
  static_assert(std::is_same_v<decltype(expected_fs::depth(std::declval<const fs::recursive_directory_iterator&>())),
                               int>);
  static_assert(
    std::is_same_v<decltype(expected_fs::recursion_pending(std::declval<const fs::recursive_directory_iterator&>())),
                   bool>);

  REQUIRE(true);
}

TEST_CASE("expected_fs exposes C++26 path formatting when available", "[api]")
{
#if EXPECTED_FS_HAS_FORMAT_PATH
  const auto formatted = std::format("{}", expected_fs::path{"expected_fs.txt"});
  REQUIRE_FALSE(formatted.empty());
  REQUIRE(formatted.find("expected_fs") != std::string::npos);
#else
  REQUIRE_FALSE(expected_fs::has_format_path);
#endif
}

TEST_CASE("expected_fs wraps common file operations", "[filesystem]")
{
  temporary_directory directory;
  const auto source = directory.path() / "source.txt";
  write_text(source, "hello");

  const auto absolute_path = expected_fs::absolute(source);
  REQUIRE(absolute_path.has_value());
  REQUIRE(absolute_path->is_absolute());

  const auto canonical_path = expected_fs::canonical(source);
  REQUIRE(canonical_path.has_value());
  const auto canonical_matches_source = expected_fs::equivalent(*canonical_path, source);
  REQUIRE(canonical_matches_source.has_value());
  REQUIRE(*canonical_matches_source);

  const auto weakly_canonical_path = expected_fs::weakly_canonical(source);
  REQUIRE(weakly_canonical_path.has_value());
  REQUIRE(weakly_canonical_path->is_absolute());

  const auto relative_path = expected_fs::relative(source, directory.path());
  REQUIRE(relative_path.has_value());
  REQUIRE(*relative_path == fs::path{"source.txt"});

  const auto proximate_path = expected_fs::proximate(source, directory.path());
  REQUIRE(proximate_path.has_value());
  REQUIRE(*proximate_path == fs::path{"source.txt"});

  const auto file_status = expected_fs::status(source);
  REQUIRE(file_status.has_value());
  REQUIRE(file_status->type() == fs::file_type::regular);
  REQUIRE(expected_fs::status_known(*file_status));
  REQUIRE(expected_fs::exists(*file_status));
  REQUIRE(expected_fs::is_regular_file(*file_status));
  REQUIRE_FALSE(expected_fs::is_directory(*file_status));
  REQUIRE_FALSE(expected_fs::is_block_file(*file_status));
  REQUIRE_FALSE(expected_fs::is_character_file(*file_status));
  REQUIRE_FALSE(expected_fs::is_fifo(*file_status));
  REQUIRE_FALSE(expected_fs::is_other(*file_status));
  REQUIRE_FALSE(expected_fs::is_socket(*file_status));
  REQUIRE_FALSE(expected_fs::is_symlink(*file_status));

  const auto file_symlink_status = expected_fs::symlink_status(source);
  REQUIRE(file_symlink_status.has_value());
  REQUIRE(file_symlink_status->type() == fs::file_type::regular);

  const auto source_exists = expected_fs::exists(source);
  REQUIRE(source_exists.has_value());
  REQUIRE(*source_exists);

  const auto source_is_regular = expected_fs::is_regular_file(source);
  REQUIRE(source_is_regular.has_value());
  REQUIRE(*source_is_regular);

  const auto source_is_directory = expected_fs::is_directory(source);
  REQUIRE(source_is_directory.has_value());
  REQUIRE_FALSE(*source_is_directory);

  const auto source_is_symlink = expected_fs::is_symlink(source);
  REQUIRE(source_is_symlink.has_value());
  REQUIRE_FALSE(*source_is_symlink);

  const auto source_is_empty = expected_fs::is_empty(source);
  REQUIRE(source_is_empty.has_value());
  REQUIRE_FALSE(*source_is_empty);

  const auto original_size = expected_fs::file_size(source);
  REQUIRE(original_size.has_value());
  REQUIRE(*original_size == std::uintmax_t{5});

  const auto link_count = expected_fs::hard_link_count(source);
  REQUIRE(link_count.has_value());
  REQUIRE(*link_count >= std::uintmax_t{1});

  const auto original_write_time = expected_fs::last_write_time(source);
  REQUIRE(original_write_time.has_value());
  REQUIRE(expected_fs::last_write_time(source, *original_write_time).has_value());

  const auto containing_space = expected_fs::space(directory.path());
  REQUIRE(containing_space.has_value());
  REQUIRE(containing_space->capacity > std::uintmax_t{0});

  REQUIRE(expected_fs::resize_file(source, std::uintmax_t{2}).has_value());
  const auto resized_size = expected_fs::file_size(source);
  REQUIRE(resized_size.has_value());
  REQUIRE(*resized_size == std::uintmax_t{2});

  const auto copied = expected_fs::copy_file(source, directory.path() / "copy.txt");
  REQUIRE(copied.has_value());
  REQUIRE(*copied);

  REQUIRE(expected_fs::rename(directory.path() / "copy.txt", directory.path() / "renamed.txt").has_value());

  const auto removed_copy = expected_fs::remove(directory.path() / "renamed.txt");
  REQUIRE(removed_copy.has_value());
  REQUIRE(*removed_copy);
}

TEST_CASE("expected_fs wraps directory_entry operations", "[filesystem]")
{
  temporary_directory directory;
  const auto first = directory.path() / "first.txt";
  const auto second = directory.path() / "second.txt";
  write_text(first, "first");
  write_text(second, "second");

  auto entry = expected_fs::make_directory_entry(first);
  REQUIRE(entry.has_value());
  REQUIRE(entry->path() == first);

  const auto entry_exists = expected_fs::exists(*entry);
  REQUIRE(entry_exists.has_value());
  REQUIRE(*entry_exists);

  const auto entry_is_regular = expected_fs::is_regular_file(*entry);
  REQUIRE(entry_is_regular.has_value());
  REQUIRE(*entry_is_regular);

  const auto entry_is_directory = expected_fs::is_directory(*entry);
  REQUIRE(entry_is_directory.has_value());
  REQUIRE_FALSE(*entry_is_directory);

  const auto entry_size = expected_fs::file_size(*entry);
  REQUIRE(entry_size.has_value());
  REQUIRE(*entry_size == std::uintmax_t{5});

  const auto entry_links = expected_fs::hard_link_count(*entry);
  REQUIRE(entry_links.has_value());
  REQUIRE(*entry_links >= std::uintmax_t{1});

  const auto entry_write_time = expected_fs::last_write_time(*entry);
  REQUIRE(entry_write_time.has_value());

  const auto entry_status = expected_fs::status(*entry);
  REQUIRE(entry_status.has_value());
  REQUIRE(expected_fs::is_regular_file(*entry_status));

  REQUIRE(expected_fs::replace_filename(*entry, second.filename()).has_value());
  REQUIRE(entry->path() == second);
  REQUIRE(expected_fs::refresh(*entry).has_value());

  REQUIRE(expected_fs::assign(*entry, first).has_value());
  REQUIRE(entry->path() == first);

  const auto missing_entry = expected_fs::make_directory_entry(directory.path() / "missing.txt");
  REQUIRE_FALSE(missing_entry.has_value());
  REQUIRE(missing_entry.error().value() != 0);
}

TEST_CASE("expected_fs wraps directory operations", "[filesystem]")
{
  temporary_directory directory;
  const auto source_root = directory.path() / "source";
  const auto nested = source_root / "a" / "b";

  const auto created_nested = expected_fs::create_directories(nested);
  REQUIRE(created_nested.has_value());
  REQUIRE(*created_nested);

  const auto created_nested_again = expected_fs::create_directories(nested);
  REQUIRE(created_nested_again.has_value());
  REQUIRE_FALSE(*created_nested_again);

  write_text(nested / "file.txt", "data");

  const auto copied_tree = expected_fs::copy(source_root, directory.path() / "copy", fs::copy_options::recursive);
  REQUIRE(copied_tree.has_value());

  const auto copied_file_exists = expected_fs::exists(directory.path() / "copy" / "a" / "b" / "file.txt");
  REQUIRE(copied_file_exists.has_value());
  REQUIRE(*copied_file_exists);

  const auto empty_directory = directory.path() / "empty";
  const auto created_empty = expected_fs::create_directory(empty_directory);
  REQUIRE(created_empty.has_value());
  REQUIRE(*created_empty);

  const auto empty_directory_is_empty = expected_fs::is_empty(empty_directory);
  REQUIRE(empty_directory_is_empty.has_value());
  REQUIRE(*empty_directory_is_empty);

  const auto empty_directory_is_directory = expected_fs::is_directory(empty_directory);
  REQUIRE(empty_directory_is_directory.has_value());
  REQUIRE(*empty_directory_is_directory);

  const auto copied_removed = expected_fs::remove_all(directory.path() / "copy");
  REQUIRE(copied_removed.has_value());
  REQUIRE(*copied_removed > std::uintmax_t{0});
}

TEST_CASE("expected_fs wraps directory iterators", "[filesystem]")
{
  temporary_directory directory;
  write_text(directory.path() / "a.txt", "a");
  write_text(directory.path() / "b.txt", "b");

  auto iterator = expected_fs::make_directory_iterator(directory.path());
  REQUIRE(iterator.has_value());

  std::set<std::string> names;
  const auto last = expected_fs::end(*iterator);
  while(*iterator != last)
  {
    names.insert((*iterator)->path().filename().string());
    REQUIRE(expected_fs::increment(*iterator).has_value());
  }

  REQUIRE(names == std::set<std::string>{"a.txt", "b.txt"});

  const auto failed_iterator = expected_fs::make_directory_iterator(directory.path() / "a.txt");
  REQUIRE_FALSE(failed_iterator.has_value());
  REQUIRE(failed_iterator.error().value() != 0);
}

TEST_CASE("expected_fs wraps recursive directory iterators", "[filesystem]")
{
  temporary_directory directory;
  const auto root = directory.path() / "root";
  const auto nested = root / "nested";
  REQUIRE(expected_fs::create_directories(nested).has_value());
  write_text(nested / "file.txt", "data");

  auto iterator = expected_fs::make_recursive_directory_iterator(root);
  REQUIRE(iterator.has_value());
  REQUIRE(expected_fs::options(*iterator) == fs::directory_options::none);
  REQUIRE(expected_fs::depth(*iterator) == 0);
  REQUIRE(expected_fs::recursion_pending(*iterator));

  expected_fs::disable_recursion_pending(*iterator);
  REQUIRE_FALSE(expected_fs::recursion_pending(*iterator));
  REQUIRE(expected_fs::increment(*iterator).has_value());
  REQUIRE(*iterator == expected_fs::end(*iterator));

  auto pop_iterator = expected_fs::make_recursive_directory_iterator(root);
  REQUIRE(pop_iterator.has_value());
  const auto recursive_last = expected_fs::end(*pop_iterator);
  while(*pop_iterator != recursive_last && expected_fs::depth(*pop_iterator) == 0)
  {
    REQUIRE(expected_fs::increment(*pop_iterator).has_value());
  }

  REQUIRE(*pop_iterator != recursive_last);
  REQUIRE(expected_fs::depth(*pop_iterator) > 0);
  REQUIRE(expected_fs::pop(*pop_iterator).has_value());
}

TEST_CASE("expected_fs wraps current path and temporary directory queries", "[filesystem]")
{
  temporary_directory directory;
  const auto original_path = fs::current_path();
  current_path_guard restore_original_path{original_path};

  const auto temp_path = expected_fs::temp_directory_path();
  REQUIRE(temp_path.has_value());
  REQUIRE(temp_path->is_absolute());

  REQUIRE(expected_fs::current_path(directory.path()).has_value());

  const auto current = expected_fs::current_path();
  REQUIRE(current.has_value());

  const auto changed_to_test_directory = expected_fs::equivalent(*current, directory.path());
  REQUIRE(changed_to_test_directory.has_value());
  REQUIRE(*changed_to_test_directory);
}

TEST_CASE("expected_fs returns error_code for filesystem failures", "[filesystem]")
{
  temporary_directory directory;
  const auto missing = directory.path() / "missing.txt";

  const auto missing_size = expected_fs::file_size(missing);
  REQUIRE_FALSE(missing_size.has_value());
  REQUIRE(missing_size.error().value() != 0);

  const auto missing_canonical = expected_fs::canonical(missing);
  REQUIRE_FALSE(missing_canonical.has_value());
  REQUIRE(missing_canonical.error().value() != 0);

  const auto missing_copy = expected_fs::copy_file(missing, directory.path() / "copy.txt");
  REQUIRE_FALSE(missing_copy.has_value());
  REQUIRE(missing_copy.error().value() != 0);

  const auto missing_remove = expected_fs::remove(missing);
  REQUIRE(missing_remove.has_value());
  REQUIRE_FALSE(*missing_remove);

  const auto missing_status = expected_fs::status(missing);
  REQUIRE(missing_status.has_value());
  REQUIRE(missing_status->type() == fs::file_type::not_found);

  const auto missing_exists = expected_fs::exists(missing);
  REQUIRE(missing_exists.has_value());
  REQUIRE_FALSE(*missing_exists);

  const auto missing_regular = expected_fs::is_regular_file(missing);
  REQUIRE(missing_regular.has_value());
  REQUIRE_FALSE(*missing_regular);

  const auto regular_file = directory.path() / "regular.txt";
  write_text(regular_file, "x");

  const auto impossible_directory = expected_fs::create_directory(regular_file / "child");
  REQUIRE_FALSE(impossible_directory.has_value());
  REQUIRE(impossible_directory.error().value() != 0);

  const auto read_regular_file_as_symlink = expected_fs::read_symlink(regular_file);
  REQUIRE_FALSE(read_regular_file_as_symlink.has_value());
  REQUIRE(read_regular_file_as_symlink.error().value() != 0);
}
