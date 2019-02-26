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
			bool read;
			bool write;
			bool execute;
		};
		
		triplet_t user;
		triplet_t group;
		triplet_t other;
		bool directory;
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
		/* Returns 0 on success */
		virtual file_t open(const buffer<char>& path, const userinfo_t userinfo) = 0;

		/* Returns 0 on success */
		virtual int64_t create(file_t locator, file_t file, const userinfo_t userinfo) = 0;

		/* Returns 0 on success */
		virtual int64_t update(file_t file, const userinfo_t userinfo) = 0;

		/* Returns a number of found files on success */
		virtual int64_t list(file_t file, fel::buffer<file_t>& data, const userinfo_t userinfo) = 0;

		/* Returns a number of byte read on success */
		virtual int64_t read(file_t locator, uint64_t offset, fel::buffer<char>& data, const userinfo_t userinfo) = 0;

		/* Returns a number of byte written on success */
		virtual int64_t write(file_t locator, uint64_t offset, fel::buffer<char>& data, const userinfo_t userinfo) = 0;

		/* Returns 0 on success */
		virtual int64_t remove(file_t locator, file_t file, const userinfo_t userinfo) = 0;
	};
}