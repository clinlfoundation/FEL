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

		struct file_attr
		{
			bool is_mount_point;
		};

		using directory_data = fel::unordered_map<file_t::fn_t, file_t, allocator, allocator>;

		/* generates a directory but do not insert it in a parent */
		[[nodiscard]] file_t mkdir(const buffer<char>& filename) const
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
				.filename_data = {},
				.filename_size = 0
			};
			fel::copy(filename, dir.filename_data);
			dir.filename_size = filename.size();
			return dir;
		}
		
		/* generates a directory and insert it in a parent, then returns the modified parent */
		[[nodiscard]] file_t mkdir(file_t parent, const buffer<char>& filename)
		{
			if(filename.size()>fel::max_file_name) return fel::badfile;

			if(parent.is_special || !parent.is_directory) return fel::badfile;

			auto dir = mkdir(filename);

			auto parent_map = reinterpret_cast<directory_data*>(parent.locator);

			parent_map->insert(dir.filename_data, dir);
			parent.size = parent_map.size();

			return parent;
		}
		
		/* generates a file and returns it */
		[[nodiscard]] file_t mkfile(const buffer<char>& filename)
		{
			if(filename.size()>fel::max_file_name) return fel::badfile;

			file_t file{
				.suid = -1,
				.guid = -1,
				.rights = permissions_t{},
				.size = 0,
				.extended = reinterpret_cast<intptr_t>(
					alloc(sizeof(file_attr))
				),
				.locator = 0,
				.is_directory = false,
				.is_special = false,
				.filename_data = {},
				.filename_size = 0
			};

			return file;
		}
		
		/* generates a file and insert it in a parent, then returns the modified parent */
		[[nodiscard]] file_t mkfile(file_t parent, const buffer<char>& filename)
		{
			if(filename.size()>fel::max_file_name) return fel::badfile;

			if(parent.is_special || !parent.is_directory) return fel::badfile;

			file_t file = mkfile(filename);

			auto parent_map = reinterpret_cast<directory_data*>(parent.locator);

			parent_map->insert(file.filename_data, file);
			parent.size = parent_map.size();

			return parent;
		}

		/* remove a non directory, non special file from a parent, then returns the modified parent*/
		[[nodiscard]] file_t rmfile(file_t parent, const file_t& file)
		{
			if(parent.is_special || !parent.is_directory) return fel::badfile;
			if(file.is_special || file.is_directory) return fel::badfile;

			auto parent_map = reinterpret_cast<directory_data*>(parent.locator);

			auto& filedata = (*parent_map)[file.filename_data];

			if(filedata.locator != 0) alloc.deallocate(reinterpret_cast<void*>(filedata.locator));
			if(filedata.extended != 0) alloc.deallocate(reinterpret_cast<void*>(filedata.extended));

			parent_map->remove(file.filename_data);
			parent.size = parent_map.size();

			return parent;
		}

		/* remove a directory from a parent, then returns the modified parent*/
		[[nodiscard]] file_t rmdir(file_t parent, const file_t& file)
		{
			if(parent.is_special || !parent.is_directory) return fel::badfile;
			if(file.is_special || !file.is_directory) return fel::badfile;

			auto parent_map = reinterpret_cast<directory_data*>(parent.locator);
			auto& filedata = (*parent_map)[file.filename_data];

			if(filedata.is_special || !filedata.is_directory) return fel::badfile;
			
			auto filemap = reinterpret_cast<directory_data*>(filedata.locator);

			for(auto& subs : *filemap)
			{
				if(subs.second.is_directory)
				{
					auto test = rmdir(filedata, subs.second);
					if(test!=fel::badfile)
					{
						filedata = test;
					}else{
						return test;
					}
				}
				else if(subs.second.is_special)
				{
					/* TODO: implement rmspecial */
				}
				else
				{
					auto test = rmfile(filedata, subs.second);
					if(test!=fel::badfile)
					{
						filedata = test;
					}else{
						return test;
					}
				}
			}

			filemap->~directory_data();

			alloc.deallocate(reinterpret_cast<void*>(filedata.locator));
			alloc.deallocate(reinterpret_cast<void*>(filedata.extended));

			parent_map->remove(file.filename_data);
			parent.size = parent_map.size();

			return parent;
		}

		enum class access_t{
			READ, WRITE, EXECUTE, UPDATE
		};

		constexpr bool has_permissions(const userinfo_t& user, const file_t& file, const access_t& type) const
		{
			switch(type)
			{
				case(access_t::READ):
					if(user.first==file.suid) return file.rights.user.read;
					for(auto group : user.second)
						if(group==file.guid) return file.rights.group.read;
					return file.rights.other.read;
				case(access_t::WRITE):
					if(user.first==file.suid) return file.rights.user.write;
					for(auto group : user.second)
						if(group==file.guid) return file.rights.group.write;
					return file.rights.other.write;
				case(access_t::EXECUTE):
					if(user.first==file.suid) return file.rights.user.execute;
					for(auto group : user.second)
						if(group==file.guid) return file.rights.group.execute;
					return file.rights.other.execute;
				case(access_t::UPDATE):
					if(user.first==file.suid) return true;
					for(auto group : user.second)
						if(group==file.guid) return true;
					return false;
			}
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