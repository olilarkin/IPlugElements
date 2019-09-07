/*=============================================================================
   Copyright (c) 2016-2019 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(CYCFI_ELEMENTS_GUI_LIB_WIDGET_DIAL_AUGUST_30_2016)
#define CYCFI_ELEMENTS_GUI_LIB_WIDGET_DIAL_AUGUST_30_2016

#include <elements/element/proxy.hpp>
#include <elements/element/tracker.hpp>
#include <elements/support.hpp>
#include <functional>

namespace cycfi { namespace elements
{
   ////////////////////////////////////////////////////////////////////////////
   // Dials
   ////////////////////////////////////////////////////////////////////////////
   class dial_base : public tracker<proxy_base>
   {
   public:

      using dial_function = std::function<void(double pos)>;
      using tracker<proxy_base>::value;

                           dial_base(double init_value = 0.0);
      virtual              ~dial_base() {}

      virtual void         prepare_subject(context& ctx);

      virtual bool         scroll(context const& ctx, point dir, point p);
      virtual void         begin_tracking(context const& ctx, info& track_info);
      virtual void         keep_tracking(context const& ctx, info& track_info);
      virtual void         end_tracking(context const& ctx, info& track_info);

      double               value() const;
      void                 value(double val);
      virtual double       value_from_point(context const& ctx, point p);

      dial_function        on_change;

   private:

      double               _value;
   };

   template <typename Subject>
   inline proxy<Subject, dial_base>
   dial(Subject&& subject, double init_value = 0.0)
   {
      return { std::forward<Subject>(subject), init_value };
   }

   inline double dial_base::value() const
   {
      return _value;
   }

   ////////////////////////////////////////////////////////////////////////////
   // Basic Knob (You can use this as the subject of dial)
   ////////////////////////////////////////////////////////////////////////////
   template <std::size_t _size>
   class basic_knob_element : public element
   {
   public:

      static std::size_t const size = _size;

                              basic_knob_element(color c = colors::black)
                               : _color(c), _value(0)
                              {}

      virtual view_limits     limits(basic_context const& ctx) const;
      virtual void            draw(context const& ctx);
      virtual void            value(double val);

   private:

      color                   _color;
      float                   _value;
   };

   template <std::size_t size>
   inline view_limits basic_knob_element<size>::limits(basic_context const& ctx) const
   {
	  auto pt = point{ float(size), float(size) };
      return view_limits{ pt, pt };
   }

   void draw_indicator(canvas& cnv, circle cp, float val, color c);

   template <std::size_t size>
   inline void basic_knob_element<size>::draw(context const& ctx)
   {
      auto& thm = get_theme();
      auto& cnv = ctx.canvas;
      auto  indicator_color = thm.indicator_color.level(1.5);
      auto  cp = circle{ center_point(ctx.bounds), ctx.bounds.width()/2 };

      draw_knob(cnv, cp, _color);
      draw_indicator(cnv, cp, _value, indicator_color);
   }

   template <std::size_t size>
   inline void basic_knob_element<size>::value(double val)
   {
      _value = val;
   }

   template <std::size_t size>
   inline basic_knob_element<size> basic_knob(color c = colors::black)
   {
      return {c};
   }

   ////////////////////////////////////////////////////////////////////////////
   // Radial Element Base (common base class for radial elements)
   ////////////////////////////////////////////////////////////////////////////
   namespace radial_consts
   {
      constexpr double _2pi = 2 * M_PI;
      constexpr double travel = 0.83;
      constexpr double range = _2pi * travel;
      constexpr double start_angle = _2pi * (1 - travel) / 2;
      constexpr double offset = (2 * M_PI) * (1 - travel) / 2;
   }

   template <std::size_t _size, typename Subject>
   class radial_element_base : public proxy<Subject>
   {
   public:

      static std::size_t const size = _size;

      using base_type = proxy<Subject>;

                              radial_element_base(Subject&& subject)
                               : base_type(std::forward<Subject>(subject))
                              {}

      virtual view_limits     limits(basic_context const& ctx) const;
      virtual void            prepare_subject(context& ctx);
   };

   template <std::size_t size, typename Subject>
   inline view_limits
   radial_element_base<size, Subject>::limits(basic_context const& ctx) const
   {
      auto sl = this->subject().limits(ctx);

      sl.min.x += size;
      sl.max.x += size;
      sl.min.y += size;
      sl.max.y += size;

      clamp_max(sl.max.x, full_extent);
      clamp_max(sl.max.y, full_extent);
      return sl;
   }

   template <std::size_t size, typename Subject>
   inline void
   radial_element_base<size, Subject>::prepare_subject(context& ctx)
   {
      auto size_div2 = size /2;
      ctx.bounds.top += size_div2;
      ctx.bounds.left += size_div2;
      ctx.bounds.bottom -= size_div2;
      ctx.bounds.right -= size_div2;
   }

   ////////////////////////////////////////////////////////////////////////////
   // Radial Marks (You can use this to place dial tick marks around dials)
   ////////////////////////////////////////////////////////////////////////////
   template <std::size_t _size, typename Subject>
   class radial_marks_element : public radial_element_base<_size, Subject>
   {
   public:

      static std::size_t const size = _size;
      using base_type = radial_element_base<_size, Subject>;
      using base_type::base_type;

      virtual void            draw(context const& ctx);
   };

   void draw_radial_marks(canvas& cnv, circle cp, float size, color c);

   template <std::size_t size, typename Subject>
   inline void
   radial_marks_element<size, Subject>::draw(context const& ctx)
   {
      // Draw the subject
      base_type::draw(ctx);

      // Draw radial lines
      auto cp = circle{ center_point(ctx.bounds), ctx.bounds.width()/2 };
      draw_radial_marks(ctx.canvas, cp, size-2, colors::light_gray);
   }

   template <std::size_t size, typename Subject>
   inline radial_marks_element<size, Subject>
   radial_marks(Subject&& subject)
   {
      return {std::forward<Subject>(subject)};
   }

   ////////////////////////////////////////////////////////////////////////////
   // Radial Labels (You can use this to place dial labels around dials)
   ////////////////////////////////////////////////////////////////////////////
   template <std::size_t _size, typename Subject, std::size_t num_labels>
   class radial_labels_element : public radial_element_base<_size, Subject>
   {
   public:

      static std::size_t const size = _size;
      using base_type = radial_element_base<_size, Subject>;
      using string_array = std::array<std::string, num_labels>;

                              radial_labels_element(Subject&& subject, float font_size)
                               : base_type(std::move(subject))
                               , _font_size(font_size)
                              {}

      virtual void            draw(context const& ctx);

      string_array            _labels;
      float                   _font_size;
   };

   void draw_radial_labels(
      canvas& cnv
    , circle cp
    , float size
    , float font_size
    , std::string const labels[]
    , std::size_t _num_labels
   );

   template <std::size_t size, typename Subject, std::size_t num_labels>
   inline void
   radial_labels_element<size, Subject, num_labels>::draw(context const& ctx)
   {
      // Draw the subject
      base_type::draw(ctx);

      // Draw the labels
      auto cp = circle{ center_point(ctx.bounds), ctx.bounds.width()/2 };
      draw_radial_labels(
         ctx.canvas, cp, size, _font_size, _labels.data(), num_labels);
   }

   template <std::size_t size, typename Subject, typename... S>
   inline radial_labels_element<size, Subject, sizeof...(S)>
   radial_labels(Subject&& subject, float font_size, S&&... s)
   {
      auto r = radial_labels_element<size, Subject, sizeof...(S)>
         {std::move(subject), font_size};
      r._labels = {{ std::move(s)... }};
      return r;
   }
}}

#endif
