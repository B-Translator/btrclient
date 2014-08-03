/**
 * @file
 *   When buttons with class 'popup' are clicked,
 *   open the result in a new window.
 */

(function($) {
  Drupal.behaviors.btrClient_popup = {
    attach: function (context, settings) {

      $('.popup').click(function(e) {
          e.preventDefault(); //prevents the default submit action
          $(this).closest('form').attr('target', '_blank').submit().removeAttr('target');
      });

    }
  };
})(jQuery);
