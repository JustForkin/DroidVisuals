/* Libvisual - The audio visualisation framework.
 * 
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Copyright (C) 2012      Chong Kai Xiong <kaixiong@codeleft.sg>
 *               2004-2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include "config.h"
#include "version.h"

#include "lv_libvisual.h"
#include "lv_common.h"

#include "lv_alpha_blend.h"
#include "lv_fourier.h"
#include "lv_plugin_registry.h"
#include "lv_log.h"
#include "lv_param.h"
#include "lv_util.h"

//#include "gettext.h"

extern "C" {
  void visual_cpu_initialize (void);
  void visual_mem_initialize (void);
}

namespace LV
{

  namespace {

    void init_params (VisParamList *params)
    {
        // Song information parameters
        visual_param_list_add_many (params,
            visual_param_new_integer ("songinfo-show",
                                      "Show song info",
                                      1,
                                      NULL),
            visual_param_new_integer ("songinfo-timeout",
                                      "Songinfo timeout in seconds",
                                      5,
                                      NULL),
            visual_param_new_bool    ("songinfo-in-plugins",
                                      "Show songinfo in plugins",
                                      TRUE,
                                      NULL),
            visual_param_new_integer ("songinfo-cover-width",
                                      "Song cover art width",
                                      128,
                                      visual_param_in_range_integer (32, 1000)),
            visual_param_new_integer ("songinfo-cover-height",
                                      "Song cover art height",
                                      128,
                                      visual_param_in_range_integer (32, 1000)),
            nullptr);
    }

  } // anonymous namespace

  class System::Impl
  {
  public:

      VisParamList *params;

      Impl ()
          : params (0)
      {}
  };

  template <>
  LV_API System* Singleton<System>::m_instance = nullptr;

  void System::init (int& argc, char**& argv)
  {
      if (!m_instance)
          m_instance = new System (argc, argv);
  }

  std::string System::get_version () const
  {
      return VISUAL_VERSION " (" LV_REVISION ")";
  }

  int System::get_api_version () const
  {
      return VISUAL_API_VERSION;
  }

  VisParamList *System::get_params () const
  {
      return m_impl->params;
  }

  System::System (int& argc, char**& argv)
      : m_impl(new Impl)
  {
      visual_log (VISUAL_LOG_INFO, "Starting Libvisual %s", get_version ().c_str ());

#if ENABLE_NLS
      //bindtextdomain (GETTEXT_PACKAGE, LOCALE_DIR);
      //bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
#endif

      // Initialize CPU caps
      visual_cpu_initialize ();

      // Initialize Mem system
      visual_mem_initialize ();

      // Initialize high-resolution timer system
      Time::init ();

      // Initialize FFT system
      Fourier::init ();

      // Initialize the plugin registry
      PluginRegistry::init ();

      m_impl->params = visual_param_list_new ();
      init_params (m_impl->params);
  }

  System::~System ()
  {
      PluginRegistry::deinit ();
      Fourier::deinit ();
      Time::deinit ();

      visual_object_unref (VISUAL_OBJECT (m_impl->params));
  }

} // LV namespace
