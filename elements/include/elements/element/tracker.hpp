/*=============================================================================
   Copyright (c) 2016-2019 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(CYCFI_ELEMENTS_GUI_LIB_WIDGET_TRACKER_AUGUST_29_2016)
#define CYCFI_ELEMENTS_GUI_LIB_WIDGET_TRACKER_AUGUST_29_2016

#include <elements/support/point.hpp>
#include <elements/element/element.hpp>
#include <memory>

namespace cycfi { namespace elements
{
   ////////////////////////////////////////////////////////////////////////////
   // tracker tracks the mouse movement.
   ////////////////////////////////////////////////////////////////////////////
   template <typename Base = element>
   class tracker : public Base
   {
   public:

      struct info
      {
         explicit          info(point start_)
                           : start(start_)
                           {}

                           info(info const&) = default;
         virtual           ~info() {}

         point             start;
         point             current = start;
         point             previous = start;
         point             offset = point{ 0, 0 };
      };

                           template <typename... T>
                           tracker(T&&... args)
                            : Base(args...)
                           {}

                           tracker(tracker const& rhs);
                           tracker(tracker&& rhs) = default;
      tracker&             operator=(tracker const& rhs);
      tracker&             operator=(tracker&& rhs) = default;

      virtual element*     click(context const& ctx, mouse_button btn);
      virtual void         drag(context const& ctx, mouse_button btn);
      virtual bool         is_control() const;

   protected:

      using info_ptr = std::unique_ptr<info>;

      virtual info_ptr     new_state(context const& ctx, point start);
      virtual void         begin_tracking(context const& ctx, info& track_info) = 0;
      virtual void         keep_tracking(context const& ctx, info& track_info) = 0;
      virtual void         end_tracking(context const& ctx, info& track_info) = 0;

   private:

      info_ptr             state;
   };

   ////////////////////////////////////////////////////////////////////////////
   // Inlines
   ////////////////////////////////////////////////////////////////////////////

   template <typename Base>
   tracker<Base>::tracker(tracker const& rhs)
    : Base(rhs)
   {}

   template <typename Base>
   tracker<Base>& tracker<Base>::operator=(tracker const& rhs)
   {
      Base::operator=(rhs);
      state.reset();
      return *this;
   }

   template <typename Base>
   inline element* tracker<Base>::click(context const& ctx, mouse_button btn)
   {
      if (btn.down)
      {
         state = new_state(ctx, btn.pos);
         begin_tracking(ctx, *state);
      }
      else if (state)
      {
         end_tracking(ctx, *state);
         state.reset();
      }
      return this;
   }

   template <typename Base>
   inline void tracker<Base>::drag(context const& ctx, mouse_button btn)
   {
      state->previous = state->current;
      state->current = btn.pos;
      state->current = state->current.move(-state->offset.x, -state->offset.y);
      keep_tracking(ctx, *state);
   }

   template <typename Base>
   inline typename tracker<Base>::info_ptr tracker<Base>::new_state(context const& ctx, point start)
   {
      return std::make_unique<info>(start);
   }

   template <typename Base>
   inline bool tracker<Base>::is_control() const
   {
      return true;
   }
}}

#endif