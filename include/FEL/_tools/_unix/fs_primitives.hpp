#pragma once
#include <FEL/array.hpp>
#include <FEL/buffer.hpp>
#include <FEL/pair.hpp>

namespace fel{

	constexpr size_t max_file_name = 255;

	using locator_t = uint64_t;

	using userinfo_t = fel::pair<int, buffer<int>>;

	struct permissions_t
	{
		struct triplet_t
		{
			bool read : 1;
			bool write : 1;
			bool execute : 1;

			constexpr bool operator==(const triplet_t& oth)
			{
				return
					read == oth.read
					&& write == oth.write
					&& execute == oth.execute;
			}
		};
		
		triplet_t user;
		triplet_t group;
		triplet_t other;
		bool directory;

		constexpr bool operator==(const permissions_t& oth)
		{
			return
				user == oth.user
				&& group == oth.group
				&& other == oth.other
				&& directory == oth.directory;
		}

		constexpr bool operator!=(const permissions_t& oth)
		{
			return !(*this==oth);
		}
	};

	struct file_t
	{
		using fn_t = fel::array<char, max_file_name>;
		static constexpr fn_t bad_filename = {};

		int suid;
		int guid;

		permissions_t rights;
		uint64_t size;
		locator_t extended;
		locator_t locator;
		bool is_directory;
		bool is_special;
		fn_t filename_data;
		size_t filename_size;

		constexpr bool operator==(const file_t& oth)
		{
			return
				suid == oth.suid
				&& guid == oth.guid
				&& rights == oth.rights
				&& extended == oth.extended
				&& locator == oth.locator
				&& is_directory == oth.is_directory
				&& is_special == oth.is_special;
		}
		constexpr bool operator!=(const file_t& oth)
		{
			return !(*this == oth);
		}
	};

	file_t badfile{
		.suid = -1,
		.guid = -1,
		.rights = permissions_t{},
		.size = 0,
		.extended = 0,
		.locator = 0,
		.is_directory = false,
		.is_special = true,
		.filename_data = file_t::bad_filename,
		.filename_size = 0
	};

	struct filesystem
	{
		static fel::pair<fel::buffer<char>, fel::buffer<char>> split_path(fel::buffer<char>& path)
		{
			for(
				auto it = path.begin();
				it != path.end();
				++it

			)
			{
				if(*it == '/')
				{
					auto current_part = fel::buffer<char>{
						&*path.begin(),
						&*it
					};
					auto pass_part = fel::buffer<char>{
						&*(++it),
						&*path.end()
					};
					return fel::pair<fel::buffer<char>,fel::buffer<char>>{current_part, pass_part};
				}
			}
			return fel::pair<fel::buffer<char>,fel::buffer<char>>{
				fel::buffer<char>{path}, 
				fel::buffer<char>{(char*)nullptr, (size_t)0}
			};
		}

		constexpr static int64_t unsuitable_location = -1;
		constexpr static int64_t already_exist = -2;
		constexpr static int64_t unavailable_operation = -3;
		constexpr static int64_t operation_failed = -4;
		constexpr static int64_t permission_denied = -13;

		/* Returns 0 on success */
		virtual file_t* open(const buffer<char>& path, const userinfo_t userinfo) = 0;

		/* Returns 0 on success */
		virtual int64_t create(file_t* locator, file_t file, const userinfo_t userinfo) = 0;

		/* Returns 0 on success */
		virtual int64_t update(file_t* file, const userinfo_t userinfo) = 0;

		/* Returns a number of found files on success */
		virtual int64_t list(file_t* file, fel::buffer<file_t>& data, const userinfo_t userinfo) = 0;

		/* Returns a number of byte read on success */
		virtual int64_t read(file_t* locator, uint64_t offset, fel::buffer<char>& data, const userinfo_t userinfo) = 0;

		/* Returns a number of byte written on success */
		virtual int64_t write(file_t* locator, uint64_t offset, fel::buffer<char>& data, const userinfo_t userinfo) = 0;

		/* Returns 0 on success */
		virtual int64_t remove(file_t* locator, file_t file, const userinfo_t userinfo) = 0;
	};
}