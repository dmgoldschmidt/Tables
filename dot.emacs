;; .emacs

;;; uncomment this line to disable loading of "default.el" at startup
;; (setq inhibit-default-init t)

;; turn on font-lock mode
(when (fboundp 'global-font-lock-mode)
  (global-font-lock-mode t))

;; enable visual feedback on selections
;(setq transient-mark-mode t)

;; repeat last shell command:
   (defun repeat-last-shell-command ()
     "Repeat last command passed to shell-command.
      From <http://www.dotemacs.de/dotfiles/KilianAFoth.emacs.html>."
     (interactive)
;     (save-buffer 'shell-command-history)
;    (or shell-command-history (error "Nothing to repeat."))
     (shell-command (car shell-command-history)))
   (global-set-key "\C-cj" 'repeat-last-shell-command)

;; default to better frame titles
(setq frame-title-format
      (concat  "%b - emacs@" system-name))

(setq-default tab-width 4)
(setq-default fill-column 78)
(setq-default major-mode 'fundamental-mode)
(setq inhibit-startup-message t)
;(display-time)
(define-key ctl-x-map "\C-b" 'electric-buffer-list)
;version control
(load-library "vc-hooks")
(setq vc-keep-workfiles t)
(setq vc-mistrust-permissions t)
(setq help-char 28)
(global-unset-key "\C-h")
(global-set-key "\C-\\" 'help-for-help)
; bind ^H to delete
(global-set-key "\C-h" 'delete-backward-char)
(if window-system
(if (not (x-display-color-p))
        (progn
  (set-face-foreground 'region "white")
          (set-face-background 'region "black"))
  (set-face-background 'modeline "yellow")
  (set-face-background 'highlight "purple")
   (set-face-foreground 'modeline "purple")
))
;(load-library "/usr/share/emacs/21.4/lisp/hilit-chg.elc")
(custom-set-variables
  ;; custom-set-variables was added by Custom.
  ;; If you edit it by hand, you could mess it up, so be careful.
  ;; Your init file should contain only one such instance.
  ;; If there is more than one, they won't work right.
 '(font-lock-mode t t (font-lock)))
  
(custom-set-faces
  ;; custom-set-faces was added by Custom.
  ;; If you edit it by hand, you could mess it up, so be careful.
  ;; Your init file should contain only one such instance.
  ;; If there is more than one, they won't work right.
 '(default ((t (:stipple nil :background "white" :foreground "black" :inverse-video nil :box nil :strike-through nil :overline nil :underline nil :slant normal :weight normal :height 120 :width normal :foundry "unknown" :family "DejaVu Sans Mono")))))
(fset 'date
   [?\C-u escape ?! ?d ?a ?t ?e return])

    (setq python-mode-hook
          '(lambda () (progn
                        (set-variable 'py-indent-offset 4)
                        (set-variable 'py-smart-indentation nil)
                        (set-variable 'indent-tabs-mode nil) )))
(setq-default indent-tabs-mode nil)
;(set-default-font "-adobe-courier-bold-r-normal--16-100-100-100-m-90-iso8859-1")
(define-key esc-map "." 'set-mark-command)
(setq mac-option-key-is-meta nil)
(setq mac-command-key-is-meta t)
(desktop-save-mode 1)

