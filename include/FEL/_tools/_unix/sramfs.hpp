#pragma once
#include <FEL/_tools/_unix/fs_primitives.hpp>
#include <FEL/memory.hpp>
#include <FEL/unordered_map.hpp>

namespace fel
{
	constexpr buffer<char> sramfs_root_name{(char*)"SRAMFSROOT", 10};
	template<typename allocator = fel::default_memory_allocator<>>
	class sramfs : public fel::filesystem
	{
		file_t root;
		allocator alloc;

		struct directory_attr
		{
			bool is_mount_point;
		};

		using directory_data = fel::unordered_map<file_t::fn_t, file_t, allocator, allocator>;

		[[nodiscard]] file_t  mkdir(const buffer<char>& filename) const
		{
			if(filename.size()>fel::max_file_name) return fel::badfile;

			file_t dir{
				.suid = -1,
				.guid = -1,
				.rights = permissions_t{},
				.size = 0,
				.extended = reinterpret_cast<intptr_t>(
					alloc(sizeof(directory_attr))
				),
				.locator = reinterpret_cast<intptr_t>(
					new(alloc(sizeof(directory_data))) directory_data(12, alloc, alloc)
				),
				.is_directory = true,
				.is_special = false,
				.filename_size = 0
			};
			fel::copy(filename, dir.filename_data);
			dir.filename_size = filename.size();
			return dir;
		}
		
		[[nodiscard]] file_t mkdir(file_t parent, const buffer<char>& filename) const
		{
			if(filename.size()>fel::max_file_name) return fel::badfile;

			if(parent.is_special || !parent.is_directory) return fel::badfile;

			auto dir = mkdir(filename);

			auto parent_map = reinterpret_cast<directory_data*>(parent.locator);

			parent_map->insert(dir.filename_data, dir);
			parent.size = parent_map.size();

			return dir;
		}

		[[nodiscard]] void rmdir(file_t file) const
		{
			
		}
	public:

		sramfs(allocator _alloc = allocator{})
		: alloc{_alloc}
		{
			root = mkdir(sramfs_root_name);
		}

		/* Returns 0 on success */
		virtual file_t open(const buffer<char>& path, const userinfo_t userinfo)
		{

		}

		/* Returns 0 on success */
		virtual int64_t create(file_t locator, file_t file, const userinfo_t userinfo)
		{

		}

		/* Returns 0 on success */
		virtual int64_t update(file_t file, const userinfo_t userinfo)
		{

		}

		/* Returns a number of found files on success */
		virtual int64_t list(file_t file, fel::buffer<file_t>& data, const userinfo_t userinfo)
		{

		}

		/* Returns a number of byte read on success */
		virtual int64_t read(file_t locator, uint64_t offset, fel::buffer<char>& data, const userinfo_t userinfo)
		{

		}

		/* Returns a number of byte written on success */
		virtual int64_t write(file_t locator, uint64_t offset, fel::buffer<char>& data, const userinfo_t userinfo)
		{

		}

		/* Returns 0 on success */
		virtual int64_t remove(file_t locator, file_t file, const userinfo_t userinfo)
		{

		}
	};

}