#pragma once
#include <FEL/_tools/_unix/fs_primitives.hpp>
#include <FEL/memory.hpp>
#include <FEL/vector.hpp>
#include <FEL/unordered_map.hpp>
#include <FEL/range/constant_range.hpp>

namespace fel
{
	namespace _fs{
		struct directory_attr
		{
			bool is_mount_point;
		};

		struct file_attr
		{
			bool is_mount_point;
		};

		struct allocation_profile
		{
			using allocator = fel::default_memory_allocator<char>;
			using directory_attr_allocator = fel::default_memory_allocator<directory_attr>;
			using file_attr_allocator = fel::default_memory_allocator<file_attr>;
			using directory_lma_allocator = fel::default_memory_allocator<node<fel::pair<file_t::fn_t, file_t>>>;
			using directory_nma_allocator = fel::default_memory_allocator<fel::pair<file_t::fn_t, file_t>>;

			using directory_data = fel::unordered_map<
				file_t::fn_t, file_t, 
				fel::hash<file_t::fn_t>, 
				directory_lma_allocator, directory_nma_allocator
			>;
			
			using file_data = fel::vector<char, allocator>;
			
			using file_data_allocator = fel::default_memory_allocator<file_data>;
			using directory_data_allocator = fel::default_memory_allocator<directory_data>;


			allocator                      alloc;
			directory_attr_allocator       dir_attr_alloc;
			file_attr_allocator            file_attr_alloc;
			directory_lma_allocator        lma_alloc;
			directory_nma_allocator        nma_alloc;
			file_data_allocator            file_alloc;
			directory_data_allocator       directory_alloc;
		};
	}

	constexpr buffer<char> sramfs_root_name{(char*)"SRAMFSROOT", 10};
	template<
		typename profile = _fs::allocation_profile
	>
	class sramfs : public fel::filesystem
	{
		file_t root;
		typename profile::allocator                      alloc;
		typename profile::directory_attr_allocator       dir_attr_alloc;
		typename profile::file_attr_allocator            file_attr_alloc;
		typename profile::directory_lma_allocator        lma_alloc;
		typename profile::directory_nma_allocator        nma_alloc;
		typename profile::file_data_allocator            file_alloc;
		typename profile::directory_data_allocator       directory_alloc;

		/* generates a directory but do not insert it in a parent */
		[[nodiscard]] file_t mkdir(const buffer<char>& filename)
		{
			if(filename.size()>fel::max_file_name) return fel::badfile;

			file_t dir{
				.suid = -1,
				.guid = -1,
				.rights = permissions_t{},
				.size = 0,
				.extended = static_cast<uint64_t>(
					reinterpret_cast<intptr_t>(
						dir_attr_alloc.allocate(1)
					)
				),
				.locator = static_cast<uint64_t>(
					reinterpret_cast<intptr_t>(
						directory_alloc.construct(directory_alloc.allocate(1), 12, lma_alloc, nma_alloc)
					)
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

			auto parent_map = reinterpret_cast<typename profile::directory_data*>(parent.locator);

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
				.extended = static_cast<uint64_t>(
					reinterpret_cast<intptr_t>(
						file_attr_alloc.allocate(1)
					)
				),
				.locator = static_cast<uint64_t>(
					reinterpret_cast<intptr_t>(
						file_alloc.construct(file_alloc.allocate(1), 0, alloc)
					)
				),
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

			auto parent_map = reinterpret_cast<typename profile::directory_data*>(parent.locator);

			parent_map->insert(file.filename_data, file);
			parent.size = parent_map.size();

			return parent;
		}

		/* remove a non directory, non special file from a parent, then returns the modified parent*/
		[[nodiscard]] file_t rmfile(file_t parent, const file_t& file)
		{
			if(parent.is_special || !parent.is_directory) return fel::badfile;
			if(file.is_special || file.is_directory) return fel::badfile;

			auto parent_map = reinterpret_cast<typename profile::directory_data*>(parent.locator);

			file_t& filedata = (*parent_map)[file.filename_data];

			if(filedata.locator != 0){
				reinterpret_cast<typename profile::file_data*>(filedata.locator)->~file_data();
				file_alloc.deallocate(reinterpret_cast<typename profile::file_data*>(filedata.locator));
			}
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

			auto parent_map = reinterpret_cast<typename profile::directory_data*>(parent.locator);
			file_t& filedata = (*parent_map)[file.filename_data];

			if(filedata.is_special || !filedata.is_directory) return fel::badfile;
			
			auto filemap = reinterpret_cast<typename profile::directory_data*>(filedata.locator);

			for(auto& subs : *filemap)
			{
				if(subs.is_directory)
				{
					auto test = rmdir(filedata, subs);
					if(test!=fel::badfile)
					{
						filedata = test;
					}else{
						return test;
					}
				}
				else if(subs.is_special)
				{
					/* TODO: implement rmspecial */
				}
				else
				{
					auto test = rmfile(filedata, subs);
					if(test!=fel::badfile)
					{
						filedata = test;
					}else{
						return test;
					}
				}
			}

			directory_alloc.destroy(filemap);

			directory_alloc.deallocate(reinterpret_cast<typename profile::directory_data*>(filedata.locator));
			dir_attr_alloc.deallocate(reinterpret_cast<_fs::directory_attr*>(filedata.extended));

			parent_map->remove(file.filename_data);
			parent.size = parent_map->size();

			return parent;
		}

		enum class access_t{
			READ, WRITE, EXECUTE, UPDATE
		};

	public:
		constexpr static permissions_t defaults_files{
			.user=permissions_t::triplet_t{
				.read=true,
				.write=true,
				.execute=false
			},
			.group=permissions_t::triplet_t{
				.read=true,
				.write=true,
				.execute=false
			},
			.other=permissions_t::triplet_t{
				.read=false,
				.write=false,
				.execute=false
			},
			.directory=false
		};
		constexpr static permissions_t defaults_directories{
			.user=permissions_t::triplet_t{
				.read=true,
				.write=true,
				.execute=false
			},
			.group=permissions_t::triplet_t{
				.read=true,
				.write=true,
				.execute=false
			},
			.other=permissions_t::triplet_t{
				.read=false,
				.write=false,
				.execute=false
			},
			.directory=false
		};

		[[nodiscard]] file_t set_permissions(const userinfo_t& user, file_t file) const
		{
			file.suid = user.first;
			file.guid = (file.guid==-1)?
				user.second.begin()
				: file.guid;
			if(file.rights==permissions_t{})
			{
				if(file.is_directory)
				{
					file.rights=defaults_directories;
				}
				else
				{
					file.rights=defaults_files;
				}
			}
			return file;
		}

		[[nodiscard]] constexpr bool has_permissions(const userinfo_t& user, const file_t& file, const access_t& type) const
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
						if(group==file.guid) return file.rights.group.write;
					return false;
			}
		}

		sramfs(profile _alloc = profile{})
		: alloc{_alloc.alloc}
		, dir_attr_alloc{_alloc.dir_attr_alloc}
		, file_attr_alloc{_alloc.file_attr_alloc}
		, lma_alloc{_alloc.lma_alloc}
		, nma_alloc{_alloc.nma_alloc}
		, file_alloc{_alloc.file_alloc}
		, directory_alloc{_alloc.directory_alloc}
		{
			root = mkdir(sramfs_root_name);
			root.is_special = true;
		}

		/* Returns a pointer to badfile on failure */
		virtual file_t* open(const buffer<char>& path, const userinfo_t userinfo)
		{
			auto cpy = path;
			file_t* explorer = &root;
			file_t::fn_t filename={};
			while(path.begin()=='/')
			{
				cpy = fel::buffer<char>{&*(++cpy.begin()), &*cpy.end()};
			}
			do{
				if(!has_permissions(userinfo, *explorer, access_t::READ)) return &fel::badfile;

				auto fname = fel::filesystem::split_path(cpy);
				auto map = reinterpret_cast<typename profile::directory_data*>(explorer->locator);

				fel::copy(
					fel::constant_range<char>('\0'),
					fel::copy(fname.first, filename)
				);
				
				explorer = &(map->get_or(filename, fel::badfile));
				if(
					explorer == &fel::badfile
					|| fname.second.size() == 0
				)
				{
					return explorer;
				}
				else
				{
					cpy = fname.second;
				}
			}while(explorer);
			return &fel::badfile;
		}

		/* Returns 0 on success */
		virtual int64_t create(file_t* locator, file_t file, const userinfo_t userinfo)
		{
			if(!has_permissions(userinfo, *locator, access_t::WRITE))
			{
				return fel::filesystem::permission_denied;
			}

			file = set_permissions(userinfo, file);

			if(locator->is_directory)
			{
				auto map = reinterpret_cast<typename profile::directory_data*>(locator->locator);
				auto filename_buf = fel::buffer{
					&*file.filename_data.begin(),
					&*file.filename_data.begin()+file.filename_size,
				};
				file_t concrete_file;
				if(file.is_directory)
				{
					concrete_file = mkdir(filename_buf);
					
				}
				else if(file.is_special)
				{
					/* TODO */
					return fel::filesystem::permission_denied;
				}
				else
				{
					concrete_file = mkfile(filename_buf);
				}
				concrete_file.rights = file.rights;
				concrete_file = set_permissions(userinfo, concrete_file);
				
				if(map->get_or(concrete_file.filename_data, fel::badfile)==fel::badfile)
				{
					map->insert(concrete_file.filename_data, concrete_file);
					return 0;
				}
				else
				{
					return fel::filesystem::already_exist;
				}
			}
			else if(locator->is_special)
			{
				/* TODO */
				return fel::filesystem::unsuitable_location;
			}else
			{
				return fel::filesystem::unavailable_operation;
			}
		}

		/* Returns 0 on success */
		virtual int64_t update(file_t* file, const userinfo_t userinfo)
		{
			return 0;
		}

		/* Returns a number of found files on success */
		virtual int64_t list(file_t* file, fel::buffer<file_t>& data, const userinfo_t userinfo)
		{
			if(!has_permissions(userinfo, *file, access_t::READ))
			{
				return fel::filesystem::permission_denied;
			}

			if(file->is_directory)
			{
				auto map = reinterpret_cast<typename profile::directory_data*>(file->locator);
				fel::copy(*map, data);
				return map->size();
			}
			else if(file->is_special)
			{
				/* TODO */
				return fel::filesystem::unavailable_operation;
			}else
			{
				return fel::filesystem::unavailable_operation;
			}
		}

		/* Returns a number of byte read on success */
		virtual int64_t read(file_t* locator, uint64_t offset, fel::buffer<char>& data, const userinfo_t userinfo)
		{
			if(!has_permissions(userinfo, *locator, access_t::READ))
			{
				return fel::filesystem::permission_denied;
			}

			if(locator->is_directory) return fel::filesystem::unavailable_operation;

			if(locator->is_special) return fel::filesystem::unavailable_operation;

			auto in_range = fel::nameless_range<typename profile::file_data::associated_iterator>{
				reinterpret_cast<typename profile::file_data*>(locator->locator)->begin()+offset,
				reinterpret_cast<typename profile::file_data*>(locator->locator)->end()
			};

			auto left = fel::copy(in_range, data);

			return data.size() - left.size();
		}

		/* Returns a number of byte written on success */
		virtual int64_t write(file_t* locator, uint64_t offset, fel::buffer<char>& data, const userinfo_t userinfo)
		{
			if(!has_permissions(userinfo, *locator, access_t::WRITE))
			{
				return fel::filesystem::permission_denied;
			}

			if(locator->is_directory) return fel::filesystem::unavailable_operation;

			if(locator->is_special) return fel::filesystem::unavailable_operation;

			auto vec = reinterpret_cast<typename profile::file_data*>(locator->locator);

			vec->resize(offset+data.size());

			auto inplace_space = fel::nameless_range<typename profile::file_data::associated_iterator>{
				vec->begin()+offset,
				vec->end()
			};

			auto source = fel::nameless_range<fel::buffer<char>::associated_iterator>{
				data.begin(), 
				data.end()
			};

			auto tmp = fel::copy(
				source,
				inplace_space
			);

			locator->size = vec->size();
			return inplace_space.size()-tmp.size();
		}

		/* Returns 0 on success */
		virtual int64_t remove(file_t* locator, file_t file, const userinfo_t userinfo)
		{
			if(
				!has_permissions(userinfo, *locator, access_t::WRITE) 
				|| !has_permissions(userinfo, file, access_t::WRITE)
			)
			{
				return fel::filesystem::permission_denied;
			}

			if(file.is_directory)
			{
				auto rev = rmdir(*locator, file);
				if(rev == fel::badfile)
					return fel::filesystem::operation_failed;
				*locator=rev;
			}
			else if(file.is_special)
			{
				/* TODO */
				return fel::filesystem::unavailable_operation;
			}
			else
			{
				auto rev = rmfile(*locator, file);
				if(rev == fel::badfile)
					return fel::filesystem::operation_failed;
				*locator=rev;
			}
			return 0;
		}
	};
}