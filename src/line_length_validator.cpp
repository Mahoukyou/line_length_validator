#include "line_length_validator.h"
#include <filesystem>
#include <string>
#include <thread>

namespace llv
{
	line_length_validator::line_length_validator(llv::validator_settings settings, std::filesystem::path path) :
		validator_settings_{ std::move(settings) },
		path_{ std::move(path) }
	{

	}


	bool line_length_validator::is_path_valid() const
	{
		using std::filesystem::exists, std::filesystem::is_regular_file, std::filesystem::is_directory;

		return exists(path()) && (is_regular_file(path()) || is_directory(path()));
	}

	void line_length_validator::validate(const bool update_directory_files)
	{
		// todo to some check for dirty cache and update only when needed without the variable
		// same thing for overview update
		if (update_directory_files)
		{
			update_files_in_directory();
		}

		// We are not going to data race in here, so no need for expensive locks
		// (unless of course something else will work on the file_validators_ while we are validating)
		// But that would happen even without the threading in here (caller screw-up, because this class is not thread-safe)

		// Poor man's threading [begin, end)
		const auto validate_job = [this](size_t begin, size_t end)
		{
			for(size_t i = begin; i < end; ++i)
			{
				file_validators_[i].validate(validator_settings());
			}
		};

		constexpr size_t thread_count{ 4 };
		if(file_validators().size() >= thread_count)
		{
			std::vector<std::thread> threads;
			threads.reserve(thread_count);

			const size_t job_size = file_validators().size() / thread_count; 
			for(size_t thread_id = 0; thread_id < thread_count; ++thread_id)
			{
				if(thread_id + 1 == thread_count)
				{
					threads.emplace_back(validate_job, thread_id * job_size, file_validators().size());
					break;
				}

				threads.emplace_back(validate_job, thread_id * job_size, (thread_id + 1) * job_size);
			}

			for(auto& thread : threads)
			{
				thread.join();
			}
		}
		else
		{
			for (auto& file_validator : file_validators_)
			{
				file_validator.validate(validator_settings());
			}
		}
	}

	void line_length_validator::update_files_in_directory()
	{
		if (!is_path_valid())
		{
			return; // todo, return type error
		}

		// todo, or just simply work on the original one?
		auto files_in_directory = files_to_validate();

		std::vector<file_validator> new_file_validators;
		new_file_validators.reserve(files_in_directory.size());

		for (auto& file_path : files_in_directory)
		{
			new_file_validators.emplace_back(std::move(file_path));
		}

		file_validators_ = std::move(new_file_validators);
	}

	void line_length_validator::update_files_overview()
	{
		for (auto& file_validator : file_validators_)
		{
			file_validator.update_overview(validator_settings());
		}
	}

	std::vector<std::filesystem::path> line_length_validator::files_to_validate() const
	{
		const auto has_extension = [this](const std::wstring file_extension) -> bool
		{
			for (const auto& extension : validator_settings().file_extensions_to_validate)
			{
				// todo, remove later, once we get extenstion to be wstring, since string cmp operator does the length compare anyway
				if (file_extension.length() != extension.length())
				{
					continue;
				}

				//todo, for now, refactor extensions later
				std::wstring tmp_ext_wstring{};
				tmp_ext_wstring.assign(extension.begin(), extension.end());

				if (file_extension == tmp_ext_wstring)
				{
					return true;
				}
			}

			return false;
		};

		// Passing a path to a file instead of a directory does not require extensions
		// Nor will abide any provided ones
		if (std::filesystem::is_regular_file(path()))
		{
			return { path() };
		}

		// todo, maybe use directory_entry instead of to get files/store them?
		// refactor later in case we need to check if our data is dirty instead of re-validating everything
		std::vector<std::filesystem::path> found_files_paths;
		for (auto& directory_entry : std::filesystem::recursive_directory_iterator(path(), std::filesystem::directory_options::skip_permission_denied))
		{
			if (directory_entry.is_regular_file() && has_extension(directory_entry.path().extension()))
			{
				found_files_paths.push_back(directory_entry.path());
			}
		}

		return found_files_paths;
	}
}