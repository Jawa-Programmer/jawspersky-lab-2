#ifndef JWDICTIONARY_H
#define JWDICTIONARY_H

#include <stdexcept>
#include <initializer_list>

namespace jpl
{
		/// класс контейнер словарь. Объекты внутри храняться в списке, по этому поиск, вставка и удаление линейны.
		/// ключи и данные хранятся по занчению
		template <class KEY, class VAL>
		class dictionary
		{
			public:
			/// структура, хранящая пару "ключ - значение"
			struct pair {KEY key; VAL value;};
			
			private:
			/// структура хранит элемент списка
			struct item {pair data; item *next;};
			/// указатель на первый элемент списка
			item *_begin; 
			/// размер списка
			int _size = 0;
			public:
			
			/// конструктор по умолчанию
			dictionary() : _begin(nullptr){}
			
			/// конструктор инициализации словаря списком пар "ключ - значение"
			dictionary(const std::initializer_list<pair>&);
			
			/// копирующий конструктор. Создаются новые item, все объекты копируются
			dictionary(const dictionary&);
			
			/// перемещающий конструктор. Пемремещает указатели на begin и end. все :)
			dictionary(dictionary&&);
			
			/// деструктор
			~dictionary();
			
			/// возвращает константную ссылку на элемент по указанному ключу. Если индекс отсутсвует, то генерируется исключение типа std::out_of_range
			const VAL& operator[](const KEY&) const;
			
			/// возвращает константную ссылку на элемент по указанному ключу. Если индекс отсутсвует, то создается новый элемент с использованием конструктора по умолчанию.
			VAL& operator[](const KEY&);
			
			/// добавляет элемент в словарь. Если в словаре уже есть элемент с заданным ключем, то значение замещается
			void set(const KEY&, const VAL&);
			
			/// элемент с заданным ключем исключается из словоря. Если элемента не было в словаре, словарь не меняется
			void unset(const KEY&);
			
			/// возвращает размер словаря (количество элементов)
			int size() const {return _size;}
			
			/// возвращает true, если словарь пуст
			bool empty() const {return _begin == nullptr;}
			
			/// удаляет все элементы в словаре
			void clear();
			
			/// меняет содержимое двух словарей местами
			void swap(dictionary& sec){
				auto tmp = _begin;
				_begin = sec._begin;
				sec._begin = tmp;
				int tmp1 = _size;
				_size = sec._size;
				sec._size = tmp1;
				}
			
			/// оператор копирующего присваивания
			dictionary& operator=(const dictionary&);
			/// оператор перемещающего присваивания
			dictionary& operator=(dictionary&&);
			
			/// класс итератор
			class iterator
			{
				private:
				const dictionary *dct;
				item *cur;
				public:
				iterator(const dictionary *_dct, item* c = nullptr) : cur(c), dct(_dct){}
				/// возвращает текущую пару "ключ - зхначение"
				pair& operator*() const { return cur -> data;}
				pair* operator->() const { return &(cur -> data);}
				iterator& operator++();
				iterator operator+(int) const;
				bool operator==(const iterator&) const;
				bool operator!=(const iterator&) const;
				
			};
			/// класс константного итератора
			class const_iterator
			{
				private:
				const dictionary *dct;
				item *cur;
				public:
				const_iterator(const dictionary *_dct, item* c = nullptr) : cur(c), dct(_dct){}
				const pair& operator*() const { return cur -> data;}
				const pair* operator->() const { return &(cur -> data);}
				const_iterator& operator++();
				const_iterator operator+(int) const;
				bool operator==(const const_iterator&) const;
				bool operator!=(const const_iterator&) const;
				
			};
			
			/// возвращает итератор ссылающийся на первый элемент
			iterator begin();
			/// возвращает итератор ссылающийся на nullptr (как бы элемент после последнего)
			iterator end();
			
			/// возвращает константный итератор ссылающийся на первый элемент
			const_iterator cbegin();
			/// возвращает константный итератор ссылающийся на nullptr (как бы элемент после последнего)
			const_iterator cend();
			
		};
				
		
		template <class KEY, class VAL>
		dictionary<KEY, VAL>::dictionary(const dictionary& old)
		{
			if(old._begin == nullptr) {
				_begin = nullptr;
				return;
			}
			dictionary<KEY, VAL>::item *old_cur = old.begin, *cur = new dictionary<KEY, VAL>::item{old_cur -> data, nullptr};
			_begin = cur;
			while(old_cur -> next)
			{
				old_cur = old_cur -> next;
				dictionary<KEY, VAL>::item *tmp = new dictionary<KEY, VAL>::item{old_cur -> data, nullptr};
				cur->next = tmp;
				cur = tmp;
			}
			_size = old.size;
		}
		
		template <class KEY, class VAL>
		dictionary<KEY, VAL>::dictionary(dictionary&& old)
		{
			_begin = old._begin;
			old._begin = nullptr;
			_size = old.size;
		}
		
		template <class KEY, class VAL>
		dictionary<KEY, VAL>::dictionary(const std::initializer_list<dictionary<KEY, VAL>::pair> &args) : _size(args.size())
		{
			if(args.size() == 0)
			{
				_begin = nullptr;
			}
			else{
				auto beg = args.begin();
				auto end = args.end();
				_begin = new dictionary<KEY, VAL>::item{{beg->key, beg->value}, nullptr};
				auto cur = _begin;
				for(++beg; beg != end; ++beg){
					cur -> next = new dictionary<KEY, VAL>::item{{beg->key, beg->value}, nullptr};
					cur = cur -> next;
				}
			}
		}
		
		template <class KEY, class VAL>
		dictionary<KEY, VAL>::~dictionary()
		{
			clear();
		}
		
		template <class KEY, class VAL>
		void dictionary<KEY, VAL>::clear()
		{
			while(_begin)
			{
				dictionary<KEY, VAL>::item *tmp = _begin -> next;
				delete _begin;
				_begin = tmp;
			}
		}
		
		template <class KEY, class VAL>
		const VAL& dictionary<KEY, VAL>::operator[](const KEY& key) const
		{
			dictionary<KEY, VAL>::item *tmp = _begin;
			while(tmp)
			{
				if(tmp->data.key == key) return tmp -> data.value;
				tmp = tmp -> next;
			}		
			throw std::out_of_range("this key was not defined");
		}
		
		
		template <class KEY, class VAL>
		VAL& dictionary<KEY, VAL>::operator[](const KEY& key)
		{
			dictionary<KEY, VAL>::item *tmp = _begin;
			while(tmp)
			{
				if(tmp->data.key == key) return tmp -> data.value;
				tmp = tmp -> next;
			}
			tmp = new dictionary<KEY, VAL>::item{key, VAL(), _begin};
			_begin = tmp;
			++_size;
			return tmp -> data.value;
		}
		
		template <class KEY, class VAL>
		void dictionary<KEY, VAL>::set(const KEY& key, const VAL& val)
		{
			dictionary<KEY, VAL>::item *tmp = _begin;
			
			while(tmp)
			{
				if(tmp->data.key == key) {
					tmp -> data.value = val;
					return;
					}
				tmp = tmp -> next;
			}		
			_begin = new dictionary<KEY, VAL>::item{{key, val}, _begin};
			++_size;
		}
		
		template <class KEY, class VAL>
		void dictionary<KEY, VAL>::unset(const KEY& key)
		{
			dictionary<KEY, VAL>::item *cur = _begin;
			if(cur -> data.key == key) 
			{
				dictionary<KEY, VAL>::item *tmp = _begin -> next;
				delete _begin;
				--_size;
				_begin = tmp;
				return;
			}
			while(cur->next)
			{
					if(cur->next->data.key == key) {
						dictionary<KEY, VAL>::item *tmp = cur -> next -> next;
						delete cur -> next;
						cur -> next = tmp;
						--_size;
						return;
					}
					cur = cur -> next;
			}	
			throw std::out_of_range("this key was not defined");			
		}
		
		template <class KEY, class VAL>
		typename dictionary<KEY, VAL>::iterator dictionary<KEY, VAL>::begin()
		{
			return dictionary<KEY, VAL>::iterator(this, _begin);
		}
		
		template <class KEY, class VAL>
		typename dictionary<KEY, VAL>::iterator dictionary<KEY, VAL>::end()
		{
			return dictionary<KEY, VAL>::iterator(this);
		}
		template <class KEY, class VAL>
		typename dictionary<KEY, VAL>::const_iterator dictionary<KEY, VAL>::cbegin()
		{
			return dictionary<KEY, VAL>::const_iterator(this, _begin);
		}
		
		template <class KEY, class VAL>
		typename dictionary<KEY, VAL>::const_iterator dictionary<KEY, VAL>::cend()
		{
			return dictionary<KEY, VAL>::const_iterator(this);
		}
		
		template <class KEY, class VAL>
		typename dictionary<KEY, VAL>::iterator& dictionary<KEY, VAL>::iterator::operator++()
		{
			if(cur){
				cur = cur -> next;
				return *this;
			}
			throw std::out_of_range("iterator out bounds");
		}
		
		template <class KEY, class VAL>
		typename dictionary<KEY, VAL>::iterator dictionary<KEY, VAL>::iterator::operator+(int k) const
		{
			if(k < 0) throw std::out_of_range("k must be greater or equal zero");
			
			dictionary<KEY, VAL>::item *tmp = cur;
			int i;
			for(i=0; (i < k) && cur; i++){
				tmp = tmp -> next;
			}
			if(i == k) return dictionary<KEY, VAL>::iterator(dct, tmp);
			throw std::out_of_range("iterator out bounds");
		}
		
		template <class KEY, class VAL>
		bool dictionary<KEY, VAL>::iterator::operator==(const  dictionary<KEY, VAL>::iterator& b) const
		{
			return cur == b.cur && dct == b.dct;
		}
		template <class KEY, class VAL>
		bool dictionary<KEY, VAL>::iterator::operator!=(const  dictionary<KEY, VAL>::iterator& b) const
		{
			return cur != b.cur || dct != b.dct;
		}
		
		
		template <class KEY, class VAL>
		bool dictionary<KEY, VAL>::const_iterator::operator==(const dictionary<KEY, VAL>::const_iterator& b) const
		{
			return cur == b.cur && dct == b.dct;
		}
		template <class KEY, class VAL>
		bool dictionary<KEY, VAL>::const_iterator::operator!=(const  dictionary<KEY, VAL>::const_iterator& b) const
		{
			return cur != b.cur || dct != b.dct;
		}
}

#endif