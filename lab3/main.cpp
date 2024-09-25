#include <cctype>
#include <exception>
#include <handleapi.h>
#include <iterator>
#include <ostream>
#include <windows.h>
#include <iostream>
#include <string>
#include <vector>

#include "binary-tree.hpp"
#include "fox.hpp"

struct ThreadContainer
{
	Fox * __fox;
	TreeNode * __tree;
	HANDLE __mutex;
};

DWORD WINAPI fox_function_thread(LPVOID lpParameter)
{
	ThreadContainer fox_data = * static_cast< ThreadContainer * >(lpParameter);
	std::cout << "start thread: " << fox_data.__fox->get_id() << std::endl;
	do
	{
		while (WaitForSingleObject(fox_data.__mutex,  INFINITE) != WAIT_OBJECT_0);
		fox_data.__fox->update();
		std::cout << "Fox: " << fox_data.__fox->get_id() << std::endl;
		draw_binary_tree(std::cout, fox_data.__tree, 6);
		std::cout << "-----------------------------------------" << std::endl;
		ReleaseMutex(fox_data.__mutex);
	}
	while (fox_data.__tree == nullptr);
	std::cout << "end thread: " << fox_data.__fox->get_id() << std::endl;
	return 0u;
}

int main(int argc, char * argv[])
{
	if (argc != 2)
	{
		std::cerr << "bad arguments\n";
		return 1;
	}

	HANDLE mutex = CreateMutexA(NULL, false, "tree");
	TreeNode * tree = nullptr;
	std::vector< Fox * > foxs;
	std::vector< HANDLE > handles;
	std::vector< DWORD > threads;
	std::vector< ThreadContainer > containers;

	try
	{
		std::size_t amount = std::stoi(argv[1]);
		for (std::size_t i = 0; i < amount; ++i)
		{
			Fox * fox = new Fox(i, nullptr, nullptr);
			auto res = push_fox(tree, fox);
			tree = res.first;
			fox->set_pos(res.second);
			foxs.push_back(fox);
			containers.push_back({fox, tree, mutex});
		}
		
		for (std::size_t i = 0; i < amount; ++i)
		{
			handles.push_back(CreateThread(NULL, 0, fox_function_thread, &containers[i], 0, threads.data() + i));
		}
		
		/*
		std::size_t i = 0;
		do
		{
			foxs[i]->update();
			std::cout << "Thread: " << foxs[i]->get_id() << "\n";
			draw_binary_tree(std::cout, tree, 6);
			std::cout << "-----------------------------------------\n\n";
			++i;
			if (i == amount) i = 0;
		}
		while (tree->__fox == nullptr);
		*/
	}
	catch (const std::bad_alloc & error)
	{
		std::cerr << error.what() << "\n";
		free_binary_tree(tree);
		return 1;
	}
	catch (const std::exception & error)
	{
		std::cerr << "unx error: " << error.what() << "\n";
		free_binary_tree(tree);
		return 1;
	}
	free_binary_tree(tree);
	return 0;
}