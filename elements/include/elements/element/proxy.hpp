/*=============================================================================
   Copyright (c) 2016-2019 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(CYCFI_ELEMENTS_GUI_LIB_PROXY_APRIL_10_2016)
#define CYCFI_ELEMENTS_GUI_LIB_PROXY_APRIL_10_2016

#include <elements/element/element.hpp>

namespace cycfi { namespace elements
{
   ////////////////////////////////////////////////////////////////////////////
   // Proxies
   //
   // A element that encapsulates another element (its 'subject'). The proxy
   // delegates its methods to its encapsulated subject, but may augment
   // or totally override its behavior.
   ////////////////////////////////////////////////////////////////////////////
   class proxy_base : public element
   {
   public:

   // Image

      virtual view_limits     limits(basic_context const& ctx) const;
      virtual view_stretch    stretch() const;
      virtual element*        hit_test(context const& ctx, point p);
      virtual void            draw(context const& ctx);
      virtual void            layout(context const& ctx);
      virtual void            refresh(context const& ctx, element& element);
      virtual bool            scroll(context const& ctx, point dir, point p);
      virtual void            prepare_subject(context& ctx);
      virtual void            prepare_subject(context& ctx, point& p);
      virtual void            restore_subject(context& ctx);

      using element::refresh;

   // Control

      virtual element*        click(context const& ctx, mouse_button btn);
      virtual void            drag(context const& ctx, mouse_button btn);
      virtual bool            key(context const& ctx, key_info k);
      virtual bool            text(context const& ctx, text_info info);
      virtual bool            cursor(context const& ctx, point p, cursor_tracking status);

      virtual bool            focus(focus_request r);
      virtual element const*  focus() const;
      virtual element*        focus();
      virtual bool            is_control() const;

   // Proxy

      virtual element const&  subject() const = 0;
      virtual element&        subject() = 0;

   // Receiver

      virtual void            value(bool val);
      virtual void            value(int val);
      virtual void            value(double val);
      virtual void            value(std::string val);
   };

   template <typename Subject, typename Base = proxy_base>
   class proxy : public Base
   {
   public:

      using subject_type = typename std::decay<Subject>::type;

                              template <typename... T>
                              proxy(Subject&& subject_, T&&... args)
                               : Base(std::forward<T>(args)...)
                               , _subject(std::forward<Subject>(subject_)) {}

      void                    subject(Subject&& subject_);
      virtual element const&  subject() const { return _subject; }
      virtual element&        subject() { return _subject; }

   private:

      subject_type            _subject;
   };

   template <typename Subject, typename Base>
   inline void proxy<Subject, Base>::subject(Subject&& subject_)
   {
      _subject = std::forward<Subject>(subject_);
   }
}}

#endif
