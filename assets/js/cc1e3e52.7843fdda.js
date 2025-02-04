"use strict";(self.webpackChunkprocedural_dungeon_wiki=self.webpackChunkprocedural_dungeon_wiki||[]).push([[21534],{15362:(e,s,o)=>{o.r(s),o.d(s,{assets:()=>c,contentTitle:()=>i,default:()=>h,frontMatter:()=>n,metadata:()=>a,toc:()=>l});const a=JSON.parse('{"id":"guides/Misc/Room-Types","title":"Room Types","description":"There is no built-in room type system provided by the plugin, because it mainly depends on the game you are creating.","source":"@site/versioned_docs/version-3.4/guides/Misc/Room-Types.md","sourceDirName":"guides/Misc","slug":"/guides/Room-Types","permalink":"/ProceduralDungeon/3.4/guides/Room-Types","draft":false,"unlisted":false,"tags":[],"version":"3.4","frontMatter":{"slug":"/guides/Room-Types","pagination_prev":null,"pagination_next":null},"sidebar":"homeSidebar"}');var t=o(74848),r=o(28453);const n={slug:"/guides/Room-Types",pagination_prev:null,pagination_next:null},i="Room Types",c={},l=[{value:"Child Room Data Classes",id:"child-room-data-classes",level:2},{value:"Variable in your Base Room Data Class",id:"variable-in-your-base-room-data-class",level:2},{value:"Arrays of <code>Room Data</code>",id:"arrays-of-room-data",level:2}];function d(e){const s={a:"a",br:"br",code:"code",h1:"h1",h2:"h2",header:"header",hr:"hr",img:"img",li:"li",p:"p",table:"table",tbody:"tbody",td:"td",th:"th",thead:"thead",tr:"tr",ul:"ul",...(0,r.R)(),...e.components};return(0,t.jsxs)(t.Fragment,{children:[(0,t.jsx)(s.header,{children:(0,t.jsx)(s.h1,{id:"room-types",children:"Room Types"})}),"\n",(0,t.jsx)(s.p,{children:"There is no built-in room type system provided by the plugin, because it mainly depends on the game you are creating."}),"\n",(0,t.jsxs)(s.p,{children:["However, you have several ways to implement it yourself.",(0,t.jsx)(s.br,{}),"\n","Here some of them:"]}),"\n",(0,t.jsxs)(s.ul,{children:["\n",(0,t.jsxs)(s.li,{children:["Hierarchy of ",(0,t.jsx)(s.code,{children:"Room Data"})," child classes"]}),"\n",(0,t.jsxs)(s.li,{children:["Variable in your base ",(0,t.jsx)(s.code,{children:"Room Data"})," class"]}),"\n",(0,t.jsxs)(s.li,{children:["Arrays of ",(0,t.jsx)(s.code,{children:"Room Data"})]}),"\n"]}),"\n",(0,t.jsx)(s.hr,{}),"\n",(0,t.jsx)(s.h2,{id:"child-room-data-classes",children:"Child Room Data Classes"}),"\n",(0,t.jsxs)(s.table,{children:[(0,t.jsx)(s.thead,{children:(0,t.jsxs)(s.tr,{children:[(0,t.jsx)(s.th,{children:"Pros"}),(0,t.jsx)(s.th,{children:"Cons"})]})}),(0,t.jsx)(s.tbody,{children:(0,t.jsxs)(s.tr,{children:[(0,t.jsx)(s.td,{children:'You can define variables specific for those types. For example, your "Shop" rooms could have a variable to tell what type of item the shop sell (clothes, food, electronic, etc.).'}),(0,t.jsx)(s.td,{children:"If you want to change the type (class) of a room data asset, you must re-create it with the new class, copy/paste shared data from the old asset to the new asset, and don't forget to change the data reference in the room level. (You can keep the same room level, you just have to change the data asset reference in it)."})]})})]}),"\n",(0,t.jsxs)(s.p,{children:["You can create different child blueprint classes of the ",(0,t.jsx)(s.code,{children:"Room Data"})," class."]}),"\n",(0,t.jsxs)(s.p,{children:["I would always recommend to create a common blueprint class deriving from the ",(0,t.jsx)(s.code,{children:"Room Data"})," class of the plugin, so you can add whatever project-specific variables in it which are shared by all your rooms."]}),"\n",(0,t.jsxs)(s.p,{children:['Then, you can create child classes of this common class for each type of room in your dungeon (for example "Shop" and "Corridor" for the dungeon example I\'ve used ',(0,t.jsx)(s.a,{href:"/ProceduralDungeon/3.4/guides/Best-Practices/Workflows/Dungeon-Generation-Algorithm",children:"here"}),")."]}),"\n",(0,t.jsx)(s.p,{children:'If you want a hierarchical type system (meaning having "groups" of types, sub-types, etc.) you can create child classes of those types. For example, you could have "Large Corridor" and "Small Corridor" deriving from the "Corridor" type.'}),"\n",(0,t.jsx)(s.p,{children:"This is what the class diagram (the hierarchy of the classes) should looks like:"}),"\n",(0,t.jsx)(s.p,{children:(0,t.jsx)(s.img,{alt:"Room Types Hierarchy",src:o(63727).A+"",width:"757",height:"514"})}),"\n",(0,t.jsx)(s.p,{children:"The dotted classes is to show the optional sub-types.\r\nHere how it looks like in your editor (blue are the child classes, pink are the asset instances of these classes) :"}),"\n",(0,t.jsx)(s.p,{children:(0,t.jsx)(s.img,{src:o(44496).A+"",width:"1029",height:"204"})}),"\n",(0,t.jsxs)(s.p,{children:["If you want to check what type of room you have in your ",(0,t.jsx)(s.code,{children:"Choose Next Room"}),", you must cast the room data into the class you want. You can then also access to the variables specific to this blueprint class if you want."]}),"\n",(0,t.jsx)(s.p,{children:(0,t.jsx)(s.img,{src:o(23816).A+"",width:"1115",height:"609"})}),"\n",(0,t.jsx)(s.hr,{}),"\n",(0,t.jsx)(s.h2,{id:"variable-in-your-base-room-data-class",children:"Variable in your Base Room Data Class"}),"\n",(0,t.jsxs)(s.table,{children:[(0,t.jsx)(s.thead,{children:(0,t.jsxs)(s.tr,{children:[(0,t.jsx)(s.th,{children:"Pros"}),(0,t.jsx)(s.th,{children:"Cons"})]})}),(0,t.jsx)(s.tbody,{children:(0,t.jsxs)(s.tr,{children:[(0,t.jsx)(s.td,{children:"You can change room type of an asset without re-creating it and you can define new types of room easily."}),(0,t.jsx)(s.td,{children:"You can't have variables specific to the room types as all the room data assets will share the same blueprint class."})]})})]}),"\n",(0,t.jsx)(s.p,{children:'If you don\'t need specific variables in each of your room types, you can use a variable to differentiate your rooms by a "type".'}),"\n",(0,t.jsx)(s.p,{children:"You can use whatever variable type you want:"}),"\n",(0,t.jsxs)(s.ul,{children:["\n",(0,t.jsxs)(s.li,{children:["\n",(0,t.jsx)(s.p,{children:"An integer is sufficient (room type 0, room type 1, etc.) but not very practical."}),"\n"]}),"\n",(0,t.jsxs)(s.li,{children:["\n",(0,t.jsxs)(s.p,{children:["A ",(0,t.jsx)(s.code,{children:"Name"})," can be used, but is error-prone because you must not make a typo when typing it in the data asset."]}),"\n"]}),"\n",(0,t.jsxs)(s.li,{children:["\n",(0,t.jsxs)(s.p,{children:["An enum is the best choice for a simple type. It's an integer variable with labels, and you have a dropdown to select it so you can't misspelled the names. However, you can't have hierarchical room type. You can set multiple types per room by making the variable an array.\r\n",(0,t.jsx)(s.img,{src:o(88126).A+"",width:"1591",height:"531"})]}),"\n"]}),"\n",(0,t.jsxs)(s.li,{children:["\n",(0,t.jsxs)(s.p,{children:["A ",(0,t.jsx)(s.code,{children:"Gameplay Tag"})," is like an enum, but also provides a hierarchical information. For example, you can create a tag ",(0,t.jsx)(s.code,{children:"Room.Corridor.Large"})," and ",(0,t.jsx)(s.code,{children:"Room.Corridor.Small"})," and you can test for any corridor type by testing the tag ",(0,t.jsx)(s.code,{children:"Room.Corridor"}),". You can also use a ",(0,t.jsx)(s.code,{children:"Gameplay Tag Container"})," variable to allow multiple types per room.\r\n",(0,t.jsx)(s.img,{src:o(20715).A+"",width:"1592",height:"528"})]}),"\n"]}),"\n",(0,t.jsxs)(s.li,{children:["\n",(0,t.jsxs)(s.p,{children:["A ",(0,t.jsx)(s.code,{children:"Data Asset"})," is also usable, like the ",(0,t.jsx)(s.code,{children:"Door Type"})," system of the plugin. You can create a child class of ",(0,t.jsx)(s.code,{children:"Data Asset"})," (we can name it ",(0,t.jsx)(s.code,{children:"Room Type"}),") then create different asset instances of the data asset class. Then you can reference these ",(0,t.jsx)(s.code,{children:"Room Type"})," data assets in a variable in your ",(0,t.jsx)(s.code,{children:"Room Data"})," assets. This solution allows you to add data to those room types (but shared by all rooms of same type).\r\n",(0,t.jsx)(s.img,{src:o(82059).A+"",width:"1588",height:"531"})]}),"\n"]}),"\n"]}),"\n",(0,t.jsx)(s.hr,{}),"\n",(0,t.jsxs)(s.h2,{id:"arrays-of-room-data",children:["Arrays of ",(0,t.jsx)(s.code,{children:"Room Data"})]}),"\n",(0,t.jsxs)(s.table,{children:[(0,t.jsx)(s.thead,{children:(0,t.jsxs)(s.tr,{children:[(0,t.jsx)(s.th,{children:"Pros"}),(0,t.jsx)(s.th,{children:"Cons"})]})}),(0,t.jsx)(s.tbody,{children:(0,t.jsxs)(s.tr,{children:[(0,t.jsxs)(s.td,{children:["You can switch the ",(0,t.jsx)(s.code,{children:"Data Asset"})," storing arrays of type in your dungeon generator so your rooms can have different types depending on the context."]}),(0,t.jsx)(s.td,{children:"You have to maintain those arrays whenever you create or delete room data assets."})]})})]}),"\n",(0,t.jsx)(s.p,{children:'This is maybe the less "clean" way of doing a room type, but can be useful in specific cases.'}),"\n",(0,t.jsxs)(s.p,{children:["You can create a new ",(0,t.jsx)(s.code,{children:"Data Asset"})," class that store arrays of ",(0,t.jsx)(s.code,{children:"Room Data"})," for each type of room you want. For example, you can have an array ",(0,t.jsx)(s.code,{children:"Corridor Rooms"})," containing all the corridor room data, an array ",(0,t.jsx)(s.code,{children:"Shop Rooms"})," with all the shop room data, etc."]}),"\n",(0,t.jsxs)(s.p,{children:["You can use the built-in ",(0,t.jsx)(s.code,{children:"Room Data"})," class directly for you room data assets, without having to create at least a common child class (but I would still recommend to do it so you can add your game-specific data in your rooms)."]}),"\n",(0,t.jsx)(s.p,{children:"It can be less performant than the other solutions when you have a lot of room data in your arrays, but this can be fixed easily by using sets instead of arrays. (sets are unordered as opposed to arrays, but are much faster to add/remove or check existence of an item in it with large amount of items)."}),"\n",(0,t.jsx)(s.p,{children:(0,t.jsx)(s.img,{src:o(35280).A+"",width:"1141",height:"544"})})]})}function h(e={}){const{wrapper:s}={...(0,r.R)(),...e.components};return s?(0,t.jsx)(s,{...e,children:(0,t.jsx)(d,{...e})}):d(e)}},63727:(e,s,o)=>{o.d(s,{A:()=>a});const a=o.p+"assets/images/RoomTypesHierarchy-4e64f572e6ec48623fe7bf0152de1bab.png"},35280:(e,s,o)=>{o.d(s,{A:()=>a});const a=o.p+"assets/images/RoomTypes_ArraysComparison-686e6f035e47bac45f7435895935b63b.png"},44496:(e,s,o)=>{o.d(s,{A:()=>a});const a=o.p+"assets/images/RoomTypes_ChildClassAssets-0ca88f9ffa344a7955c39efc20b57afe.png"},23816:(e,s,o)=>{o.d(s,{A:()=>a});const a=o.p+"assets/images/RoomTypes_ChildClassComparison-932df56fcd0e63f89f0415b661bd2303.png"},82059:(e,s,o)=>{o.d(s,{A:()=>a});const a=o.p+"assets/images/RoomTypes_VarDataAssetComparison-77344c7b3238672b882a19ae499c74b8.png"},88126:(e,s,o)=>{o.d(s,{A:()=>a});const a=o.p+"assets/images/RoomTypes_VarEnumComparison-3ea3c4291c6ddf692875b1e44f5c8a15.png"},20715:(e,s,o)=>{o.d(s,{A:()=>a});const a=o.p+"assets/images/RoomTypes_VarGameplayTagComparison-0185b9623bee79105d5e64d2e163da3a.png"},28453:(e,s,o)=>{o.d(s,{R:()=>n,x:()=>i});var a=o(96540);const t={},r=a.createContext(t);function n(e){const s=a.useContext(r);return a.useMemo((function(){return"function"==typeof e?e(s):{...s,...e}}),[s,e])}function i(e){let s;return s=e.disableParentContext?"function"==typeof e.components?e.components(t):e.components||t:n(e.components),a.createElement(r.Provider,{value:s},e.children)}}}]);