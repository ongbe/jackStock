!function(t,e,n){function i(t){var e,n,i,r,a={};if(e=t.indexOf("?"),0>e)return a;for(n=t.substring(e+1).split("&"),i=n.length;i--;)r=n[i].split("="),a[r[0]]=r[1];return a}function r(t,n,i){e.addEventListener?t.addEventListener(n,i,!1):attachEvent&&t.attachEvent("on"+n,i)}function a(t,n,i){e.removeEventListener?t.removeEventListener(n,i):detachEvent&&t.detachEvent("on"+n,i)}function o(t){this.url=t,this.map=i(this.url)}function c(){var t=(new Date).getTime();P+=t-E,E=t}function d(t){var e=new Image;O&&(e.onload=function(){1==e.height&&1==e.width&&(O=!1)}),e.src=t}function u(t){var e=D.getParam("crid"),n=t.locHash;if(!e||!n)return"";n=new o("?"+(t.locHash[e]||"#").replace(/#/,""));var i=n.getParam("hvsid");return i?"&hvsid="+i:""}function v(){if("undefined"==typeof t._mNL2)return"";var e=t._mNL2.util.getVisitorId();return e?"&vsid="+e:""}function m(t){if(!t||!t.persisted){var e=[L,"?hlf=9&ts=",P,O?"&limp=1":"","&cid=",D.getParam("cid"),"&crid=",D.getParam("crid"),"&pid=",D.getParam("pid"),"&purl=",N,I.visitID,I.visitorID,"&bc=",Math.ceil(1e3*Math.random())].join("");P=0,d(e)}}function s(){y?(c(),m()):E=(new Date).getTime(),y=!1,setTimeout(s,M)}function f(){y=!0}function l(){try{return t.top!=t?t.top:t}catch(e){return null}}function h(){var t=l();null===t||"undefined"!=typeof n.emInit||100*Math.random()>T||(D=new o(w),I.visitID=u(n),I.visitorID=v(),n.emInit=!0,r(t,"scroll",f),r(t,"mousemove",f),r(t,"click",f),r(t,"pagehide",m),s())}function p(){a(e,"DOMContentLoaded",p),a(t,"load",p),f(),h()}function g(){return/interactive|complete|loaded/.test(e.readyState)?void p():(r(e,"DOMContentLoaded",p),void r(t,"load",p))}var D,L=n.emDataLogServer,w=t.iframeURL,I={},E=(new Date).getTime(),P=0,y=!1,M=n.emDataSendInterval,T=n.emLogPercent,N=n.requrl,O=!0;o.prototype.getParam=function(t){return this.map[t]||""},g()}(window,document,_mNDetails);