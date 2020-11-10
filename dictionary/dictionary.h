#ifndef JWDICTIONARY_H
#define JWDICTIONARY_H

#include <stdexcept>

namespace jpl
{
		/// класс контейнер словарь. Объекты внутри храняться в списке, по этому поиск, вставка и удаление линейны.
		template <class KEY, class VAL>
		class dictionary
		{
			private:
			struct item {const KEY *key; const VAL *value; item *next;};
			item *_begin;
			public:
			/// конструктор по умолчанию
			dictionary() : _begin(nullptr){}
			
			/// копирующий конструктор. Создаются новые item, но указатели на ключи и значение в них те же.
			dictionary(const dictionary&);
			
			/// перемещающий конструктор. Пемремещает указатели на begin и end. все :)
			dictionary(dictionary&&);
			
			/// деструктор
			~dictionary();
			
			/// возвращает элемент по указанному индексу. Если индекс отсутсвует, то генерируется исключение типа std::out_of_range
			VAL operator[](const KEY&) const;
			
			/// добавляет элемент в словарь. Если в словаре уже есть элемент с заданным ключем, то значение замещается
			void set(const KEY&, const VAL&);
			
			/// элемент с заданным ключем исключается из словоря. Если элемента не было в словаре, словарь не меняется
			void unset(const KEY&);
			
			/// класс итератор
			class iterator
			{
				private:
				item *cur;
				public:
				iterator(item* c = nullptr) : cur(c){}
				const VAL& operator*() const { return *(cur -> value);}
				/// разыменовывает ключ
				const KEY& operator-() const { return *(cur -> key);}
				iterator& operator++();
				iterator operator+(int) const;
				bool operator==(const iterator&) const;
				bool operator!=(const iterator&) const;
				
			};
			/// возвращает итератор ссылающийся на первый элемент
			iterator begin();
			/// возвращает итератор ссылающийся на nullptr (как бы элемент после последнего)
			iterator end();
			
		};
				
		
		template <class KEY, class VAL>
		dictionary<KEY, VAL>::dictionary(const dictionary& old)
		{
			if(old._begin == nullptr) {
				_begin = nullptr;
				return;
			}
			dictionary<KEY, VAL>::item *old_cur = old.begin, *cur = new dictionary<KEY, VAL>::item{old_cur -> key, old_cur -> value, nullptr};
			_begin = cur;
			while(old_cur -> next)
			{
				old_cur = old_cur -> next;
				dictionary<KEY, VAL>::item *tmp = new dictionary<KEY, VAL>::item{old_cur -> key, old_cur -> value, nullptr};
				cur->next = tmp;
				cur = tmp;
			}
		}
		
		template <class KEY, class VAL>
		dictionary<KEY, VAL>::dictionary(dictionary&& old)
		{
			_begin = old._begin;
			old._begin = nullptr;
		}
		
		template <class KEY, class VAL>
		dictionary<KEY, VAL>::~dictionary()
		{
			while(_begin)
			{
				dictionary<KEY, VAL>::item *tmp = _begin -> next;
				delete _begin;
				_begin = tmp;
			}
		}
		
		template <class KEY, class VAL>
		VAL dictionary<KEY, VAL>::operator[](const KEY& key) const
		{
			dictionary<KEY, VAL>::item *tmp = _begin;
			while(tmp)
			{
				if(*(tmp->key) == key) return *(tmp->value);
				tmp = tmp -> next;
			}		
			throw new std::out_of_range("this key was not defined");
		}
		
		template <class KEY, class VAL>
		void dictionary<KEY, VAL>::set(const KEY& key, const VAL& val)
		{
			dictionary<KEY, VAL>::item *tmp = _begin;
			
			while(tmp)
			{
				if(*(tmp->key) == key) {
					tmp -> value = &val;
					return;
					}
				tmp = tmp -> next;
			}		
			_begin = new dictionary<KEY, VAL>::item{&key, &val, _begin};
		}
		
		template <class KEY, class VAL>
		void dictionary<KEY, VAL>::unset(const KEY& key)
		{
			dictionary<KEY, VAL>::item *cur = _begin;
			if(*(cur -> key) == key) 
			{
				dictionary<KEY, VAL>::item *tmp = _begin -> next;
				delete _begin;
				_begin = tmp;
				return;
			}
			while(cur->next)
			{
					if(*(cur->next->key) == key) {
						dictionary<KEY, VAL>::item *tmp = cur -> next -> next;
						delete cur -> next;
						cur -> next = tmp;
						return;
					}
					cur = cur -> next;
			}		
		}
		
		template <class KEY, class VAL>
		typename dictionary<KEY, VAL>::iterator dictionary<KEY, VAL>::begin()
		{
			return dictionary<KEY, VAL>::iterator(_begin);
		}
		
		template <class KEY, class VAL>
		typename dictionary<KEY, VAL>::iterator dictionary<KEY, VAL>::end()
		{
			return dictionary<KEY, VAL>::iterator();
		}
		
		template <class KEY, class VAL>
		typename dictionary<KEY, VAL>::iterator& dictionary<KEY, VAL>::iterator::operator++()
		{
			if(cur){
				cur = cur -> next;
				return *this;
			}
			throw new std::out_of_range("iterator out bounds");
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
			if(i == k) return dictionary<KEY, VAL>::iterator(tmp);
			throw new std::out_of_range("iterator out bounds");
		}
		
		template <class KEY, class VAL>
		bool dictionary<KEY, VAL>::iterator::operator==(const  dictionary<KEY, VAL>::iterator& b) const
		{
			return cur == b.cur;
		}
		template <class KEY, class VAL>
		bool dictionary<KEY, VAL>::iterator::operator!=(const  dictionary<KEY, VAL>::iterator& b) const
		{
			return cur != b.cur;
		}
		
}

#endif